#include "utils.h"

#include <windows.h>
#include <tchar.h>
#include <tlhelp32.h>

void ShowCriticalError(LPTSTR message)
{
	MessageBox(NULL, message, _T("OpenGL/DirectX Overlay - Critical error"), MB_OK | MB_ICONERROR | MB_TOPMOST | MB_TASKMODAL);
	ExitProcess(1);
}

bool PauseAllThreads()
{
	DWORD process_id = GetCurrentProcessId();
	DWORD current_thread_id = GetCurrentThreadId();

	HANDLE process_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (process_snapshot == INVALID_HANDLE_VALUE)
		return false;

	THREADENTRY32 current_thread;
	current_thread.dwSize = sizeof (THREADENTRY32);

	if (!Thread32First(process_snapshot, &current_thread))
		return false;

	while (true)
	{
		if (current_thread.th32OwnerProcessID == process_id && current_thread.th32ThreadID != current_thread_id)
		{
			HANDLE thread = OpenThread(THREAD_ALL_ACCESS, FALSE, current_thread.th32ThreadID);
			if (thread == NULL)
				return false;

			if (SuspendThread(thread) == (DWORD) -1)
				return false;

			if (!CloseHandle(thread))
				return false;
		}

		if (!Thread32Next(process_snapshot, &current_thread))
		{
			if (GetLastError() == ERROR_NO_MORE_FILES)
				break;

			return false;
		}
	}

	if (!CloseHandle(process_snapshot))
		return false;

    return true;
}


bool ResumeAllThreads()
{
	DWORD process_id = GetCurrentProcessId();
	DWORD current_thread_id = GetCurrentThreadId();

	HANDLE process_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, process_id);
	if (process_snapshot == INVALID_HANDLE_VALUE)
		return false;

	THREADENTRY32 current_thread;
	current_thread.dwSize = sizeof (THREADENTRY32);

	if (!Thread32First(process_snapshot, &current_thread))
		return false;

	while (true)
	{
		if (current_thread.th32OwnerProcessID == process_id && current_thread.th32ThreadID != current_thread_id)
		{
			HANDLE thread = OpenThread(THREAD_ALL_ACCESS, FALSE, current_thread.th32ThreadID);
			if (thread == NULL)
				return false;

			if (ResumeThread(thread) == (DWORD) -1)
				return false;

			if (!CloseHandle(thread))
				return false;
		}

		if (!Thread32Next(process_snapshot, &current_thread))
		{
			if (GetLastError() == ERROR_NO_MORE_FILES)
				break;

			return false;
		}
	}

	if (!CloseHandle(process_snapshot))
		return false;

    return true;
}
