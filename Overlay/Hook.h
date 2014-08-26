#ifndef HOOKS_H
#define HOOKS_H

#include "data_types.h"

class Hook
{
private:
	MaxUInt target_function;
	MaxUInt replacement_function;
	MaxUInt bridge_address;

	MaxUInt jmp_size;
	MaxUInt bridge_size;

	bool enabled;

public:
	Hook(MaxUInt target_function, MaxUInt replacement_function);
	Hook();
	~Hook();

	bool setParameters(MaxUInt target_function, MaxUInt replacement_function);

	bool enable();
	bool disable();

	MaxUInt getBridge();

private:
	bool GetInstructionLength(MaxUInt address, MaxUInt & length);
	bool GetRegionBoundaries(MaxUInt address, MaxUInt & base_address, MaxUInt & total_length, MaxUInt & remaining_length);
	void AssembleJumpTo(MaxUInt destination, MaxUInt jump_target);
};

#endif // HOOKS_H
