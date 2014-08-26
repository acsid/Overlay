#include "directx9_support.h"

#include <windows.h>
#include <tchar.h>

#include "Hook.h"
#include "utils.h"
#include "directx9_main.h"

#include <hash_map>
using namespace std;

#define DX9_VTABLE_RELEASE				0x02
#define DX9_VTABLE_PRESENT				0x11
#define DX9_VTABLE_RESET				0x10
#define DX9_VTABLE_PRESENTEX			0x79
#define DX9X_VTABLE_RESETEX				0x84
#define DX9_VTABLE_SWAPCHAIN_PRESENT	0x03

HMODULE d3d9_module;
Hook present_hook;
Hook presentex_hook;
Hook release_hook;
Hook swapchain_present_hook;
Hook reset_hook;
Hook resetex_hook;
HANDLE initialization_thread;
hash_map<IDirect3DDevice9 *, DX9OverlayMainBase *> known_devices;

static DWORD WINAPI InitializationThread_P(LPVOID reserved);
static void PresentScene_P(IDirect3DDevice9 *device);

typedef HRESULT (STDMETHODCALLTYPE * IDirect3DSwapChain9PresentFunctionType)(IDirect3DSwapChain9 *, CONST RECT *, CONST RECT *, HWND, CONST RGNDATA *, DWORD);
IDirect3DSwapChain9PresentFunctionType IDirect3DSwapChain9Present;
HRESULT STDMETHODCALLTYPE IDirect3DSwapChain9_Present_Replacement_P(IDirect3DSwapChain9 *swap_chain, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion, DWORD dwFlags);

HRESULT STDMETHODCALLTYPE IDirect3DDevice9Ex_PresentEx_Replacement_P(IDirect3DDevice9Ex *device, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion, DWORD dwFlags);
typedef HRESULT (STDMETHODCALLTYPE * IDirect3DDevice9ExPresentExFunctionType)(IDirect3DDevice9 *, CONST RECT *, CONST RECT *, HWND, CONST RGNDATA *, DWORD);
IDirect3DDevice9ExPresentExFunctionType IDirect3DDevice9ExPresentEx;

HRESULT STDMETHODCALLTYPE IDirect3DDevice9_Present_Replacement_P(IDirect3DDevice9 *device, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion);
typedef HRESULT (STDMETHODCALLTYPE * IDirect3DDevice9PresentFunctionType)(IDirect3DDevice9 *, CONST RECT *, CONST RECT *, HWND, CONST RGNDATA *);
IDirect3DDevice9PresentFunctionType IDirect3DDevice9Present;

HRESULT STDMETHODCALLTYPE IDirect3DDevice9_Reset_Replacement_P(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *pPresentationParameters);
typedef HRESULT (STDMETHODCALLTYPE * IDirect3DDevice9ResetFunctionType)(IDirect3DDevice9 *, D3DPRESENT_PARAMETERS *);
IDirect3DDevice9ResetFunctionType IDirect3DDevice9Reset;
	
HRESULT STDMETHODCALLTYPE IDirect3DDevice9Ex_ResetEx_Replacement_P(IDirect3DDevice9Ex *device, D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode);
typedef HRESULT (STDMETHODCALLTYPE * IDirect3DDevice9ExResetExFunctionType)(IDirect3DDevice9Ex *, D3DPRESENT_PARAMETERS *, D3DDISPLAYMODEEX *);
IDirect3DDevice9ExResetExFunctionType IDirect3DDevice9ExResetEx;

ULONG STDMETHODCALLTYPE IUnknown_Release_Replacement_P(void *iunknown_object);
typedef ULONG (STDMETHODCALLTYPE * IUnknownReleaseFunctionType)(void *);
IUnknownReleaseFunctionType IUnknownRelease;

bool WINAPI IsDirectX9Loaded(bool & found)
{
	d3d9_module = GetModuleHandle(_T("d3d9.dll"));
	if (d3d9_module == NULL)
		found = false;
	else
		found = true;

	return true;
}

bool WINAPI DirectX9Initialize()
{
	// we can't create the DirectX device from DllMain. There is no need to synchronize, because the thread will run AFTER
	// the DLL entry point has returned. This is because all DllMain calls are serialized, and creating a thread would require
	// our library to be called again with DLL_THREAD_ATTACH

	if (!PauseAllThreads())
		ShowCriticalError(_T("Could not pause the threads"));

	initialization_thread = CreateThread(NULL, 0, InitializationThread_P, NULL, 0, NULL);
	if (initialization_thread == NULL)
	{
		if (!ResumeAllThreads())
			ShowCriticalError(_T("Could not resume the threads"));

		return false;
	}

	return true;
}

void WINAPI DirectX9Cleanup()
{
	if (!CloseHandle(initialization_thread))
		ShowCriticalError(_T("Could not release the thread handle"));

	HMODULE d3d9_module = GetModuleHandle(_T("d3d9.dll"));
	if (d3d9_module != NULL)
	{
		if (!swapchain_present_hook.disable())
			ShowCriticalError(_T("The DirectX 9 hooks could not be removed"));

		if (!presentex_hook.disable())
			ShowCriticalError(_T("The DirectX 9 hooks could not be removed"));

		if (!present_hook.disable())
			ShowCriticalError(_T("The DirectX 9 hooks could not be removed"));

		if (!release_hook.disable())
			ShowCriticalError(_T("The DirectX 9 hooks could not be removed"));
	}
}

static DWORD WINAPI InitializationThread_P(LPVOID reserved)
{
	d3d9_module = GetModuleHandle(_T("d3d9.dll"));

	WNDCLASSEX window_class;
	ZeroMemory(&window_class, sizeof (WNDCLASSEX));

	window_class.cbSize = sizeof (WNDCLASSEX);
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = DefWindowProc;
	window_class.lpszClassName = _T("TempDirectXOverlayWindow");

	window_class.hInstance = GetModuleHandle(NULL);
	if (window_class.hInstance == NULL)
		ShowCriticalError(_T("Could not get the instance handle"));

	if (!RegisterClassEx(&window_class))
		ShowCriticalError(_T("Could not get register the window class"));

	HWND temp_window = CreateWindow(window_class.lpszClassName, _T("Temporary DirectX Overlay Window"), WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, window_class.hInstance, NULL);
	if (temp_window == NULL)
		ShowCriticalError(_T("Could not get create the temporary window"));

	LPVOID Direct3DCreate9Ex = (LPVOID) GetProcAddress(d3d9_module, "Direct3DCreate9Ex");
	if (Direct3DCreate9Ex == NULL)
		ShowCriticalError(_T("Could not locate the Direct3DCreate9 procedure entry point"));

	IDirect3D9Ex *d3d9_ex;
	HRESULT error_code = ((HRESULT (WINAPI *)(UINT, IDirect3D9Ex **)) Direct3DCreate9Ex)(D3D_SDK_VERSION, &d3d9_ex);
	if (FAILED(error_code))
		ShowCriticalError(_T("Could not create the DirectX 9 interface"));

	D3DDISPLAYMODE display_mode;
	if (FAILED(d3d9_ex->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &display_mode)))
		ShowCriticalError(_T("Could not determine the current display mode"));

	D3DPRESENT_PARAMETERS present_parameters;
	ZeroMemory(&present_parameters, sizeof (D3DPRESENT_PARAMETERS));
	present_parameters.Windowed = TRUE;
	present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	present_parameters.BackBufferFormat = display_mode.Format;

	IDirect3DDevice9Ex *d3d9_device_ex;
	error_code = d3d9_ex->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, temp_window, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &present_parameters, NULL, &d3d9_device_ex);
	if (FAILED(error_code))
		ShowCriticalError(_T("Could not create the Direct3D 9 device"));

#ifdef _M_IX86
	UInt32 *vtable = *((UInt32 **) d3d9_device_ex);
#else
	UInt64 *vtable = *((UInt64 **) d3d9_device_ex);
#endif

	release_hook.setParameters((MaxUInt) vtable[DX9_VTABLE_RELEASE], (MaxUInt) IUnknown_Release_Replacement_P);

	present_hook.setParameters((MaxUInt) vtable[DX9_VTABLE_PRESENT], (MaxUInt) IDirect3DDevice9_Present_Replacement_P);
	reset_hook.setParameters((MaxUInt) vtable[DX9_VTABLE_RESET], (MaxUInt) IDirect3DDevice9_Reset_Replacement_P);

	presentex_hook.setParameters((MaxUInt) vtable[DX9_VTABLE_PRESENTEX], (MaxUInt) IDirect3DDevice9Ex_PresentEx_Replacement_P);
	resetex_hook.setParameters((MaxUInt) vtable[DX9X_VTABLE_RESETEX], (MaxUInt) IDirect3DDevice9Ex_ResetEx_Replacement_P);

	IDirect3DSwapChain9 *swap_chain;
	if (FAILED(d3d9_device_ex->GetSwapChain(0, &swap_chain)))
		ShowCriticalError(_T("Could not obtain the swap chain"));

#ifdef _M_IX86
	UInt32 *swapchain_vtable = *((UInt32 **) swap_chain);
#else
	UInt64 *swapchain_vtable = *((UInt64 **) swap_chain);
#endif

	swapchain_present_hook.setParameters((MaxUInt) swapchain_vtable[DX9_VTABLE_SWAPCHAIN_PRESENT], (MaxUInt) IDirect3DSwapChain9_Present_Replacement_P);

	swap_chain->Release();
	d3d9_device_ex->Release();
	d3d9_ex->Release();

	if (!DestroyWindow(temp_window))
		ShowCriticalError(_T("Could not release the temporary window"));

	if (!UnregisterClass(window_class.lpszClassName, window_class.hInstance))
		ShowCriticalError(_T("Could not release the window class"));

	if (!swapchain_present_hook.enable())
		ShowCriticalError(_T("The DirectX 9 hooks could not be enabled"));

	if (!presentex_hook.enable())
		ShowCriticalError(_T("The DirectX 9 hooks could not be enabled"));

	if (!present_hook.enable())
		ShowCriticalError(_T("The DirectX 9 hooks could not be enabled"));

	if (!release_hook.enable())
		ShowCriticalError(_T("The DirectX 9 hooks could not be enabled"));

	if (!reset_hook.enable())
		ShowCriticalError(_T("The DirectX 9 hooks could not be enabled"));

	if (!resetex_hook.enable())
		ShowCriticalError(_T("The DirectX 9 hooks could not be enabled"));

	IDirect3DSwapChain9Present = (IDirect3DSwapChain9PresentFunctionType) swapchain_present_hook.getBridge();
	IDirect3DDevice9ExPresentEx = (IDirect3DDevice9ExPresentExFunctionType) presentex_hook.getBridge();
	IDirect3DDevice9Present = (IDirect3DDevice9PresentFunctionType) present_hook.getBridge();
	IUnknownRelease = (IUnknownReleaseFunctionType) release_hook.getBridge();
	IDirect3DDevice9Reset = (IDirect3DDevice9ResetFunctionType) reset_hook.getBridge();
	IDirect3DDevice9ExResetEx = (IDirect3DDevice9ExResetExFunctionType) resetex_hook.getBridge();

	if (!ResumeAllThreads())
		ShowCriticalError(_T("The threads could not be resumed"));

	return 0;
}

HRESULT STDMETHODCALLTYPE IDirect3DSwapChain9_Present_Replacement_P(IDirect3DSwapChain9 *swap_chain, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion, DWORD dwFlags)
{
	IDirect3DDevice9 *device;
	HRESULT error_code = swap_chain->GetDevice(&device);
	if (FAILED(error_code))
		return error_code;

	PresentScene_P(device);

	return IDirect3DSwapChain9Present(swap_chain, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
}

HRESULT STDMETHODCALLTYPE IDirect3DDevice9Ex_PresentEx_Replacement_P(IDirect3DDevice9Ex *device, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion, DWORD dwFlags)
{
	PresentScene_P(device);

	return IDirect3DDevice9ExPresentEx(device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
}

HRESULT STDMETHODCALLTYPE IDirect3DDevice9_Present_Replacement_P(IDirect3DDevice9 *device, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion)
{
	PresentScene_P(device);

	return IDirect3DDevice9Present(device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT STDMETHODCALLTYPE IDirect3DDevice9_Reset_Replacement_P(IDirect3DDevice9 *device, D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	hash_map<IDirect3DDevice9 *, DX9OverlayMainBase *>::iterator current_device = known_devices.find(device);
	if (current_device == known_devices.end())
		return IDirect3DDevice9Reset(device, pPresentationParameters);

	DX9OverlayMainBase *overlay = current_device->second;
	if (overlay->isInitialialized() == true)
	{
		overlay->unloadContent();
		overlay->setInitialized(false);
	}

	return IDirect3DDevice9Reset(device, pPresentationParameters);
}

HRESULT STDMETHODCALLTYPE IDirect3DDevice9Ex_ResetEx_Replacement_P(IDirect3DDevice9Ex *device, D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode)
{
	if (device == NULL)
		return D3DERR_DEVICELOST;

	hash_map<IDirect3DDevice9 *, DX9OverlayMainBase *>::iterator current_device = known_devices.find(device);
	if (current_device != known_devices.end())
		return IDirect3DDevice9ExResetEx(device, pPresentationParameters, pFullscreenDisplayMode);

	DX9OverlayMainBase *overlay = current_device->second;
	if (overlay->isInitialialized() == true)
	{
		overlay->unloadContent();
		overlay->setInitialized(false);
	}

	return IDirect3DDevice9ExResetEx(device, pPresentationParameters, pFullscreenDisplayMode);
}

ULONG STDMETHODCALLTYPE IUnknown_Release_Replacement_P(void *iunknown_object)
{
	// the order of what we do here is *REALLY* important to avoid issues
	//
	// 1. determine if this is a known device object
	// 2. determine if the object is going to be deleted
	// 3. erase the device from the hash_map. if we leave it there, overlay->unloadContent()
	//    will make users of the device (i.d.: vertex buffers, textures...) call release again
	//    on the device. if it happens, we'll skip the first IF block because we find a device
	//    we're already releasing.
	// 4. unload our content
	// 5. call the release method

	IDirect3DDevice9 *device = (IDirect3DDevice9 *) iunknown_object;
	hash_map<IDirect3DDevice9 *, DX9OverlayMainBase *>::iterator current_device = known_devices.find(device);
	if (current_device == known_devices.end())
		return IUnknownRelease(iunknown_object);

	device->AddRef();
	if (IUnknownRelease(iunknown_object) == 2)
	{
		DX9OverlayMainBase *overlay = current_device->second;
		known_devices.erase(current_device);

		if (overlay->isInitialialized() == true)
			overlay->unloadContent();

		delete overlay;
	}

	return IUnknownRelease(iunknown_object);
}

static void PresentScene_P(IDirect3DDevice9 *device)
{
	DX9OverlayMainBase *current_overlay;
	hash_map<IDirect3DDevice9 *, DX9OverlayMainBase *>::iterator current_device = known_devices.find(device);
	if (current_device == known_devices.end())
	{
		current_overlay = new DX9OverlayMain;
		current_overlay->setDevice(device);
		current_overlay->setInitialized(false);
		known_devices[device] = current_overlay;
	}
	else
		current_overlay = current_device->second;

	if (current_overlay->isInitialialized() == false)
	{
		current_overlay->loadContent();
		current_overlay->setInitialized(true);
	}

	current_overlay->draw();
}

void DX9OverlayMainBase::setDevice(IDirect3DDevice9 *device)
{
	this->device = device;
}

IDirect3DDevice9 *DX9OverlayMainBase::getDevice()
{
	return device;
}
void DX9OverlayMainBase::setInitialized(bool initialized)
{
	this->initialized = initialized;
}

bool DX9OverlayMainBase::isInitialialized()
{
	return initialized;
}
