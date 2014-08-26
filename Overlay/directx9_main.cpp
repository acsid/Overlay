#include "directx9_main.h"

void DX9OverlayMain::loadContent()
{
	CUSTOMVERTEX vertices[] =
	{
		{ 150.0f, 50.0f, 0.5f, 1.0f, 0xffff0000, },
		{ 250.0f, 250.0f, 0.5f, 1.0f, 0xff00ff00, },
		{ 50.0f, 250.0f, 0.5f, 1.0f, 0xff00ffff, }
	};

    if (FAILED(device->CreateVertexBuffer(3 * sizeof (CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &vertex_buffer, NULL)))
		return;

    void *locked_buffer;
    if (FAILED(vertex_buffer->Lock(0, sizeof (vertices), (void **) &locked_buffer, 0)))
        return;

    memcpy(locked_buffer, vertices, sizeof (vertices));
    vertex_buffer->Unlock();
}

void DX9OverlayMain::unloadContent()
{
	if (vertex_buffer != NULL)
	{
		vertex_buffer->Release();
		vertex_buffer = NULL;
	}
}

void DX9OverlayMain::draw()
{
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    device->SetRenderState(D3DRS_LIGHTING, FALSE);
    device->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	device->SetTexture(0, NULL);

	if (FAILED(device->BeginScene()))
		return;

	device->SetStreamSource(0, vertex_buffer, 0, sizeof (CUSTOMVERTEX));
	device->SetFVF(D3DFVF_CUSTOMVERTEX);
	device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

	device->EndScene();
}
