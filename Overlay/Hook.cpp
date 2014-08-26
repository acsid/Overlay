#include "Hook.h"

#include <windows.h>
#include <crtdbg.h>
#include <distorm.h>

Hook::Hook()
{
#ifdef _M_IX86
	jmp_size = 5;
#else
	jmp_size = 14;
#endif

	enabled = false;
	target_function = 0LL;
	replacement_function = 0LL;
	bridge_address = 0LL;
}

Hook::~Hook()
{
}

bool Hook::setParameters(MaxUInt target_function, MaxUInt replacement_function)
{
	if (enabled)
		return false;

	this->target_function = target_function;
	this->replacement_function = replacement_function;
	bridge_address = 0LL;

	return true;
}

bool Hook::enable()
{
	if (enabled)
		return true;

	MaxUInt base_address, total_length, remaining_length;
	if (!GetRegionBoundaries(target_function, base_address, total_length, remaining_length))
		return false;

	DWORD previous_protection;
	if (!VirtualProtect((LPVOID) target_function, (SIZE_T) remaining_length, PAGE_EXECUTE_READWRITE, &previous_protection))
		return false;

	MaxUInt instr_to_copy = 0;
	do
	{
		MaxUInt instruction_length = remaining_length - instr_to_copy;
		if (!GetInstructionLength(target_function + instr_to_copy, instruction_length))
			return false;

		instr_to_copy += instruction_length;
	} while (instr_to_copy < jmp_size);

	bridge_size = instr_to_copy + jmp_size;

	UInt8 *bridge = (UInt8 *) VirtualAlloc(NULL, (SIZE_T) bridge_size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (bridge == NULL)
		return false;

	UInt8 *function_buffer = (UInt8 *) target_function;
	for (MaxUInt i = 0; i < instr_to_copy; i++)
		bridge[i] = function_buffer[i];

	AssembleJumpTo(((MaxUInt) bridge) + instr_to_copy, target_function + instr_to_copy);
	AssembleJumpTo(target_function, replacement_function);

	if (!VirtualProtect((LPVOID) target_function, (SIZE_T) remaining_length, previous_protection, &previous_protection))
		return false;

	if (!FlushInstructionCache(GetCurrentProcess(), (LPCVOID) target_function, (SIZE_T) jmp_size))
		return false;

	if (!FlushInstructionCache(GetCurrentProcess(), (LPCVOID) bridge, (SIZE_T) bridge_size))
		return false;

	enabled = true;
	bridge_address = (MaxUInt) bridge;

	return true;
}

bool Hook::disable()
{
	if (!enabled)
		return true;

	DWORD previous_protection;
	if (!VirtualProtect((LPVOID) bridge_address, (SIZE_T) bridge_size, PAGE_EXECUTE_READWRITE, &previous_protection))
		return false;

	if (!VirtualProtect((LPVOID) target_function, (SIZE_T) jmp_size, PAGE_EXECUTE_READWRITE, &previous_protection))
		return false;

	UInt8 *source = (UInt8 *) bridge_address;
	UInt8 *destination = (UInt8 *) target_function;

	for (MaxUInt i = 0; i < bridge_size - jmp_size; i++)
		destination[i] = source[i];

	if (!VirtualProtect((LPVOID) target_function, (SIZE_T) jmp_size, previous_protection, &previous_protection))
		return false;

	if (!VirtualFree((LPVOID) bridge_address, 0, MEM_RELEASE))
		return false;

	if (!FlushInstructionCache(GetCurrentProcess(), (LPCVOID) target_function, (SIZE_T) jmp_size))
		return false;

	bridge_address = 0;
	enabled = false;

	return true;
}

MaxUInt Hook::getBridge()
{
	if (!enabled)
		return 0LL;

	return bridge_address;
}

bool Hook::GetInstructionLength(MaxUInt address, MaxUInt & length)
{
	//
	// change this code to use your preferred disasm engine
	//

	_OffsetType offset = 0;
	_DecodeResult decode_result;
	_DecodedInst decoded_instruction;
	unsigned int decoded_instr_count;

#ifdef _M_IX86
	_DecodeType decode_type = Decode32Bits;
#else
	_DecodeType decode_type = Decode64Bits;
#endif

	decode_result = distorm_decode(offset, (const unsigned char*) address, (int) length, decode_type, &decoded_instruction, 1, &decoded_instr_count);
	if (decode_result == DECRES_INPUTERR)
		return false;

	length = (MaxUInt) decoded_instruction.size;

	return true;
}

bool Hook::GetRegionBoundaries(MaxUInt address, MaxUInt & base_address, MaxUInt & total_length, MaxUInt & remaining_length)
{
	MEMORY_BASIC_INFORMATION memory_info;

	if (!VirtualQuery((LPVOID) address, &memory_info, sizeof (memory_info)))
		return false;

	base_address = (MaxUInt) memory_info.BaseAddress;
	total_length = (MaxUInt) memory_info.RegionSize;
	remaining_length = (base_address + total_length) - address;

	return true;
}

void Hook::AssembleJumpTo(MaxUInt destination, MaxUInt jump_target)
{
#ifdef _M_IX86
	// jmp jump_target
	*((UInt8 *) destination) = 0xE9;
	destination++;

	UInt32 *destination_address = (UInt32 *) destination;
	*destination_address = (UInt32) (jump_target - destination - 4);
#else
	UInt32 low_jump_target = (UInt32) jump_target;
	UInt32 high_jump_target = (UInt32) (jump_target >> 32);
	UInt8 *destination_buffer = (UInt8 *) destination;

	// push low_jump_target
	*destination_buffer = 0x68;
	destination_buffer++;

	*((UInt32 *) destination_buffer) = low_jump_target;
	destination_buffer += sizeof (UInt32);

	// mov dword ss:[rsp + 0x04], high_jump_target
	*destination_buffer = 0xC7;
	destination_buffer++;

	*destination_buffer = 0x44;
	destination_buffer++;

	*destination_buffer = 0xE4;
	destination_buffer++;

	*destination_buffer = 0x04;
	destination_buffer++;

	*((UInt32 *) destination_buffer) = high_jump_target;
	destination_buffer += sizeof (UInt32);

	// retn
	*destination_buffer = 0xC3;
#endif
}
