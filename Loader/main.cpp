#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <tlhelp32.h>
#include <psapi.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include <vector>
using namespace std;

bool terminate_monitor = false;
TCHAR overlay_path[MAX_PATH + 1];

INT _tmain(INT argc, LPTSTR argv[], LPTSTR lpenv[]);
void ErrorMessage(LPTSTR message, bool terminate = true);
BOOL WINAPI CtrlHandler(DWORD type);
DWORD WINAPI ProcessMonitor(LPVOID reserved);
int LoadOverlay(DWORD process_id);

INT _tmain(INT argc, LPTSTR argv[], LPTSTR lpenv[])
{
	_tprintf(_T("This loader will scan the system in search for DirectX or OpenGL games. Once a suitable process is found, the overlay module will be loaded."));
	_tprintf(_T("Press CTRL+C to terminate this program.\n"));

	TCHAR current_directory[MAX_PATH + 1];
	if (!GetCurrentDirectory(MAX_PATH, current_directory))
		return false;

	if (FAILED(StringCchPrintf(overlay_path, (sizeof (overlay_path) / sizeof (TCHAR)) - 1, _T("%s\\%s"), current_directory, _T("Overlay.dll"))))
		return false;

	_tprintf(_T("Overlay module path: %s\n\n"), overlay_path);

	HANDLE overlay_module = CreateFile(overlay_path, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (overlay_module == INVALID_HANDLE_VALUE)
		ErrorMessage(_T("The overlay module could not be found. Make sure the \"Overlay.dll\" file is located in the same folder as this loader (and that the working directory is right)"));

	if (!CloseHandle(overlay_module))
		ErrorMessage(_T("Could not release the handle"));

	HANDLE standard_input = GetStdHandle(STD_INPUT_HANDLE);
	if (standard_input == INVALID_HANDLE_VALUE)
		ErrorMessage(_T("Could not get the standard input handle"));

	DWORD console_mode;
	if (!GetConsoleMode(standard_input, &console_mode))
		ErrorMessage(_T("Could not get the console mode"));

	if (!SetConsoleMode(standard_input, console_mode | ENABLE_PROCESSED_INPUT))
		ErrorMessage(_T("Could not set the console mode"));

	if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
		ErrorMessage(_T("Could not set the CTRL+C handler routine"));

	HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ProcessMonitor, NULL, 0, NULL);
	if (thread == NULL)
		ErrorMessage(_T("Could not create the process monitoring thread"));

	if (WaitForSingleObject(thread, INFINITE) == WAIT_FAILED)
		ErrorMessage(_T("Synchronization error"));

	if (!CloseHandle(thread))
		ErrorMessage(_T("Could not release the thread handle"));

	_tprintf(_T("Press return to quit..."));
	getchar();

	return 0;
}

void ErrorMessage(LPTSTR message, bool terminate)
{
	_tprintf(_T("An error has occurred"));

	if (terminate)
		_tprintf(_T(" and the program must be terminated"));

	_tprintf(_T(".\n"));
	_tprintf(_T("Error message: %s\n"), message);

	_tprintf(_T("Press return to continue..."));
	getchar();

	if (terminate)
		ExitProcess(1);
}

BOOL WINAPI CtrlHandler(DWORD type)
{
	if (type != CTRL_C_EVENT && type != CTRL_CLOSE_EVENT)
		return FALSE;

	if (!terminate_monitor)
	{
		terminate_monitor = true;
		_tprintf(_T("Terminating the process monitor...\n"));
	}

	return TRUE;
}

DWORD WINAPI ProcessMonitor(LPVOID reserved)
{
	vector<DWORD> scanned_processes;

	while (!terminate_monitor)
	{
		HANDLE system_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (system_snapshot == INVALID_HANDLE_VALUE)
		{
			if (GetLastError() != ERROR_BAD_LENGTH)
				ErrorMessage(_T("Could not create a system snapshot"));
		}

		PROCESSENTRY32 current_process;
		current_process.dwSize = sizeof (PROCESSENTRY32);

		if (!Process32First(system_snapshot, &current_process))
			ErrorMessage(_T("Could not enumerate the running processes"));

		while (true)
		{
			bool skip_process = false;

			// simple blacklist
			if (lstrcmpi(current_process.szExeFile, _T("steam.exe")) == 0 ||
				lstrcmpi(current_process.szExeFile, _T("skype.exe")) == 0 ||
				lstrcmpi(current_process.szExeFile, _T("firefox.exe")) == 0 ||
				lstrcmpi(current_process.szExeFile, _T("chrome.exe")) == 0 ||
				lstrcmpi(current_process.szExeFile, _T("vcexpress.exe")) == 0 ||
				lstrcmpi(current_process.szExeFile, _T("dwm.exe")) == 0 ||
				lstrcmpi(current_process.szExeFile, _T("gameoverlayui.exe")) == 0)
			{
				skip_process = true;
			}

			for (vector<DWORD>::size_type i = 0; i < scanned_processes.size(); i++)
			{
				if (scanned_processes.at(i) == current_process.th32ProcessID)
				{
					skip_process = true;
					break;
				}
			}

			if (!skip_process)
			{
				if (LoadOverlay(current_process.th32ProcessID) == 0)
				{
					_tprintf(_T("Overlay loaded: %s (0x%X/%d)\n"), current_process.szExeFile, current_process.th32ProcessID, current_process.th32ProcessID);
					scanned_processes.push_back(current_process.th32ProcessID);
				}
			}

			if (!Process32Next(system_snapshot, &current_process))
			{
				if (GetLastError() == ERROR_NO_MORE_FILES)
					break;

				ErrorMessage(_T("Could not enumerate the running processes"));
			}
		}

		if (!CloseHandle(system_snapshot))
			ErrorMessage(_T("Could not release the system snapshot"));

		Sleep(1000);
	}

	return 0;
}

int LoadOverlay(DWORD process_id)
{
	// return codes
	// -1: no error (we probably don't have access to the process)
	// 0: injected
	// 1: error
	// 2: overlay already loaded

	HANDLE process_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id);
	if (process_snapshot == NULL)
		return -1;

	MODULEENTRY32 current_module;
	current_module.dwSize = sizeof (MODULEENTRY32);

	if (!Module32First(process_snapshot, &current_module))
	{
		if (!CloseHandle(process_snapshot))
			return 1;

		return -1;
	}

	bool overlay_found = false;
	while (true)
	{
		if (StrStrI(current_module.szExePath, _T("Overlay.dll")) != NULL)
		{
			overlay_found = true;
			break;
		}

		if (!Module32Next(process_snapshot, &current_module))
		{
			if (GetLastError() == ERROR_NO_MORE_FILES)
				break;

			CloseHandle(process_snapshot);

			return 1;
		}
	}

	if (!CloseHandle(process_snapshot))
		return 1;

	if (overlay_found)
		return 2;

	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	if (process == NULL)
		return -1;

	LPVOID remote_buffer = VirtualAllocEx(process, NULL, sizeof (overlay_path), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (remote_buffer == NULL)
		return 1;

	SIZE_T byte_written;
	if (!WriteProcessMemory(process, remote_buffer, overlay_path, sizeof (overlay_path), &byte_written))
		return 1;

	LPVOID loadlibrary_address;
	if (sizeof (TCHAR) == 1)
		loadlibrary_address = GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "LoadLibraryA");
	else
		loadlibrary_address = GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "LoadLibraryW");

	DWORD exit_code;
	bool error = true;

	HANDLE remote_thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE) loadlibrary_address, remote_buffer, 0, NULL);
	if (remote_thread != NULL)
	{
		if (WaitForSingleObject(remote_thread, INFINITE) == WAIT_FAILED)
			return 1;

		if (!GetExitCodeThread(remote_thread, &exit_code))
			return 1;

		if (!CloseHandle(remote_thread))
			return 1;

		error = false;
	}

	if (!VirtualFreeEx(process, remote_buffer, 0, MEM_RELEASE))
		return 1;

	if (!CloseHandle(process))
		return 1;

	if (error)
		return -1;

	if (exit_code == 0)
		return 1;

	return 0;
}
