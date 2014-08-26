#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <crtdbg.h>

#include "errors.h"
#include "data_types.h"

#include "opengl_support.h"
#include "directx9_support.h"

MODULE_CLEANUP_ROUTINE cleanup_routine;

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason_code, LPVOID reserved);
bool Initialize();

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason_code, LPVOID reserved)
{
	switch (reason_code)
	{
		case DLL_PROCESS_ATTACH:
		{
			if (!Initialize())
				return FALSE;

			return TRUE;
		}

		case DLL_PROCESS_DETACH:
		{
			if (cleanup_routine != NULL)
				cleanup_routine();

			return TRUE;
		}

		default:
			return TRUE;
	}
}

bool Initialize()
{
	MODULE_SEARCH_ROUTINE module_search_routines[] =
	{
		IsOpenGLLoaded,
		IsDirectX9Loaded
	};

	MODULE_INIT_ROUTINE module_init_routines[] =
	{
		OpenGLInitialize,
		DirectX9Initialize
	};

	MODULE_CLEANUP_ROUTINE module_cleanup_routines[] =
	{
		OpenGLCleanup,
		DirectX9Cleanup
	};

	int routine_count = sizeof (module_search_routines) / sizeof (MODULE_SEARCH_ROUTINE);
	_ASSERT(routine_count == (sizeof (module_init_routines) / sizeof (MODULE_INIT_ROUTINE)));

	MODULE_INIT_ROUTINE init_routine = NULL;
	cleanup_routine = NULL;

	for (int i = 0; i < routine_count; i++)
	{
		MODULE_SEARCH_ROUTINE search_routine = module_search_routines[i];

		bool found;
		if (!search_routine(found))
			return false;

		if (found)
		{
			init_routine = module_init_routines[i];
			cleanup_routine = module_cleanup_routines[i];

			break;
		}
	}

	if (init_routine == NULL)
		return false;

	return init_routine();
}
