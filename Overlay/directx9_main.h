#ifndef DIRECTX9_MAIN_H
#define DIRECTX9_MAIN_H

#include <windows.h>
#include <d3d9.h>

#include "directx9_support.h"

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
struct CUSTOMVERTEX
{
    FLOAT x, y, z, rhw;
    DWORD color;
};

class DX9OverlayMain : public DX9OverlayMainBase
{
	LPDIRECT3DVERTEXBUFFER9 vertex_buffer;

public:
	virtual void loadContent();
	virtual void unloadContent();

	virtual void draw();
};

#endif // DIRECTX9_MAIN_H
