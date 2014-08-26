#ifndef DIRECTX9_SUPPORT_H
#define DIRECTX9_SUPPORT_H

#include <windows.h>
#include <d3d9.h>

bool WINAPI IsDirectX9Loaded(bool & found);
bool WINAPI DirectX9Initialize();
void WINAPI DirectX9Cleanup();

class DX9OverlayMainBase
{
protected:
	bool initialized;
	IDirect3DDevice9 *device;

public:
	void setDevice(IDirect3DDevice9 *device);
	IDirect3DDevice9 *getDevice();

	void setInitialized(bool initialized);
	bool isInitialialized();

	virtual void loadContent() = NULL;
	virtual void unloadContent() = NULL;

	virtual void draw() = NULL;
};

#endif DIRECTX9_SUPPORT_H
