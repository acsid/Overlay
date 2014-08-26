#include "opengl_support.h"

#include <windows.h>
#include <tchar.h>
#include <math.h>

#include "Hook.h"
#include "opengl_main.h"
#include "utils.h"

OPENGL_CONTEXT opengl_context;
Hook swap_buffers_hook;
bool initialize_opengl;

BOOL WINAPI SwapBuffersReplacement_P(HDC hdc);
bool GetOpenGLAPI_P(OPENGL_CONTEXT & opengl_context);

bool WINAPI IsOpenGLLoaded(bool & found)
{
	if (GetModuleHandle(_T("Opengl32.dll")) == NULL)
		found = false;
	else
		found = true;

	return true;
}

bool WINAPI OpenGLInitialize()
{
	HMODULE gdi_module = GetModuleHandle(_T("Gdi32.dll"));
	if (gdi_module == NULL)
		return false;

	if (!GetOpenGLAPI_P(opengl_context))
		return false;

	MaxUInt swapbuffers_address = (MaxUInt) GetProcAddress(gdi_module, "SwapBuffers");
	swap_buffers_hook.setParameters(swapbuffers_address, (MaxUInt) SwapBuffersReplacement_P);
	if (!swap_buffers_hook.enable())
		ShowCriticalError(_T("The OpenGL hooks could not be enabled"));

	initialize_opengl = true;

	return true;
}

void WINAPI OpenGLCleanup()
{
	if (initialize_opengl)
		return;

	UnloadContent(opengl_context);
	initialize_opengl = true;

	HMODULE gdi = GetModuleHandle(_T("Gdi32.dll"));
	if (gdi != NULL)
	{
		if (!swap_buffers_hook.disable())
			ShowCriticalError(_T("The OpenGL hooks could not be disabled"));
	}
}

BOOL WINAPI SwapBuffersReplacement_P(HDC hdc)
{
	if (initialize_opengl)
	{
		LoadContent(opengl_context);
		initialize_opengl = false;
	}

	Draw(opengl_context);

	return ((BOOL (WINAPI *)(HDC)) swap_buffers_hook.getBridge())(hdc);
}

bool GetOpenGLAPI_P(OPENGL_CONTEXT & opengl_context)
{
	HMODULE opengl_module = GetModuleHandle(_T("Opengl32.dll"));
	if (opengl_module == NULL)
		return false;

	opengl_context.glAccum = (OPENGL_FUNC_glAccum) GetProcAddress(opengl_module, "glAccum");
	if (opengl_context.glAccum == NULL)
		return false;

	opengl_context.glAlphaFunc = (OPENGL_FUNC_glAlphaFunc) GetProcAddress(opengl_module, "glAlphaFunc");
	if (opengl_context.glAlphaFunc == NULL)
		return false;

	opengl_context.glAreTexturesResident = (OPENGL_FUNC_glAreTexturesResident) GetProcAddress(opengl_module, "glAreTexturesResident");
	if (opengl_context.glAreTexturesResident == NULL)
		return false;

	opengl_context.glArrayElement = (OPENGL_FUNC_glArrayElement) GetProcAddress(opengl_module, "glArrayElement");
	if (opengl_context.glArrayElement == NULL)
		return false;

	opengl_context.glBegin = (OPENGL_FUNC_glBegin) GetProcAddress(opengl_module, "glBegin");
	if (opengl_context.glBegin == NULL)
		return false;

	opengl_context.glBindTexture = (OPENGL_FUNC_glBindTexture) GetProcAddress(opengl_module, "glBindTexture");
	if (opengl_context.glBindTexture == NULL)
		return false;

	opengl_context.glBitmap = (OPENGL_FUNC_glBitmap) GetProcAddress(opengl_module, "glBitmap");
	if (opengl_context.glBitmap == NULL)
		return false;

	opengl_context.glBlendFunc = (OPENGL_FUNC_glBlendFunc) GetProcAddress(opengl_module, "glBlendFunc");
	if (opengl_context.glBlendFunc == NULL)
		return false;

	opengl_context.glCallList = (OPENGL_FUNC_glCallList) GetProcAddress(opengl_module, "glCallList");
	if (opengl_context.glCallList == NULL)
		return false;

	opengl_context.glCallLists = (OPENGL_FUNC_glCallLists) GetProcAddress(opengl_module, "glCallLists");
	if (opengl_context.glCallLists == NULL)
		return false;

	opengl_context.glClear = (OPENGL_FUNC_glClear) GetProcAddress(opengl_module, "glClear");
	if (opengl_context.glClear == NULL)
		return false;

	opengl_context.glClearAccum = (OPENGL_FUNC_glClearAccum) GetProcAddress(opengl_module, "glClearAccum");
	if (opengl_context.glClearAccum == NULL)
		return false;

	opengl_context.glClearColor = (OPENGL_FUNC_glClearColor) GetProcAddress(opengl_module, "glClearColor");
	if (opengl_context.glClearColor == NULL)
		return false;

	opengl_context.glClearDepth = (OPENGL_FUNC_glClearDepth) GetProcAddress(opengl_module, "glClearDepth");
	if (opengl_context.glClearDepth == NULL)
		return false;

	opengl_context.glClearIndex = (OPENGL_FUNC_glClearIndex) GetProcAddress(opengl_module, "glClearIndex");
	if (opengl_context.glClearIndex == NULL)
		return false;

	opengl_context.glClearStencil = (OPENGL_FUNC_glClearStencil) GetProcAddress(opengl_module, "glClearStencil");
	if (opengl_context.glClearStencil == NULL)
		return false;

	opengl_context.glClipPlane = (OPENGL_FUNC_glClipPlane) GetProcAddress(opengl_module, "glClipPlane");
	if (opengl_context.glClipPlane == NULL)
		return false;

	opengl_context.glColor3b = (OPENGL_FUNC_glColor3b) GetProcAddress(opengl_module, "glColor3b");
	if (opengl_context.glColor3b == NULL)
		return false;

	opengl_context.glColor3bv = (OPENGL_FUNC_glColor3bv) GetProcAddress(opengl_module, "glColor3bv");
	if (opengl_context.glColor3bv == NULL)
		return false;

	opengl_context.glColor3d = (OPENGL_FUNC_glColor3d) GetProcAddress(opengl_module, "glColor3d");
	if (opengl_context.glColor3d == NULL)
		return false;

	opengl_context.glColor3dv = (OPENGL_FUNC_glColor3dv) GetProcAddress(opengl_module, "glColor3dv");
	if (opengl_context.glColor3dv == NULL)
		return false;

	opengl_context.glColor3f = (OPENGL_FUNC_glColor3f) GetProcAddress(opengl_module, "glColor3f");
	if (opengl_context.glColor3f == NULL)
		return false;

	opengl_context.glColor3fv = (OPENGL_FUNC_glColor3fv) GetProcAddress(opengl_module, "glColor3fv");
	if (opengl_context.glColor3fv == NULL)
		return false;

	opengl_context.glColor3i = (OPENGL_FUNC_glColor3i) GetProcAddress(opengl_module, "glColor3i");
	if (opengl_context.glColor3i == NULL)
		return false;

	opengl_context.glColor3iv = (OPENGL_FUNC_glColor3iv) GetProcAddress(opengl_module, "glColor3iv");
	if (opengl_context.glColor3iv == NULL)
		return false;

	opengl_context.glColor3s = (OPENGL_FUNC_glColor3s) GetProcAddress(opengl_module, "glColor3s");
	if (opengl_context.glColor3s == NULL)
		return false;

	opengl_context.glColor3sv = (OPENGL_FUNC_glColor3sv) GetProcAddress(opengl_module, "glColor3sv");
	if (opengl_context.glColor3sv == NULL)
		return false;

	opengl_context.glColor3ub = (OPENGL_FUNC_glColor3ub) GetProcAddress(opengl_module, "glColor3ub");
	if (opengl_context.glColor3ub == NULL)
		return false;

	opengl_context.glColor3ubv = (OPENGL_FUNC_glColor3ubv) GetProcAddress(opengl_module, "glColor3ubv");
	if (opengl_context.glColor3ubv == NULL)
		return false;

	opengl_context.glColor3ui = (OPENGL_FUNC_glColor3ui) GetProcAddress(opengl_module, "glColor3ui");
	if (opengl_context.glColor3ui == NULL)
		return false;

	opengl_context.glColor3uiv = (OPENGL_FUNC_glColor3uiv) GetProcAddress(opengl_module, "glColor3uiv");
	if (opengl_context.glColor3uiv == NULL)
		return false;

	opengl_context.glColor3us = (OPENGL_FUNC_glColor3us) GetProcAddress(opengl_module, "glColor3us");
	if (opengl_context.glColor3us == NULL)
		return false;

	opengl_context.glColor3usv = (OPENGL_FUNC_glColor3usv) GetProcAddress(opengl_module, "glColor3usv");
	if (opengl_context.glColor3usv == NULL)
		return false;

	opengl_context.glColor4b = (OPENGL_FUNC_glColor4b) GetProcAddress(opengl_module, "glColor4b");
	if (opengl_context.glColor4b == NULL)
		return false;

	opengl_context.glColor4bv = (OPENGL_FUNC_glColor4bv) GetProcAddress(opengl_module, "glColor4bv");
	if (opengl_context.glColor4bv == NULL)
		return false;

	opengl_context.glColor4d = (OPENGL_FUNC_glColor4d) GetProcAddress(opengl_module, "glColor4d");
	if (opengl_context.glColor4d == NULL)
		return false;

	opengl_context.glColor4dv = (OPENGL_FUNC_glColor4dv) GetProcAddress(opengl_module, "glColor4dv");
	if (opengl_context.glColor4dv == NULL)
		return false;

	opengl_context.glColor4f = (OPENGL_FUNC_glColor4f) GetProcAddress(opengl_module, "glColor4f");
	if (opengl_context.glColor4f == NULL)
		return false;

	opengl_context.glColor4fv = (OPENGL_FUNC_glColor4fv) GetProcAddress(opengl_module, "glColor4fv");
	if (opengl_context.glColor4fv == NULL)
		return false;

	opengl_context.glColor4i = (OPENGL_FUNC_glColor4i) GetProcAddress(opengl_module, "glColor4i");
	if (opengl_context.glColor4i == NULL)
		return false;

	opengl_context.glColor4iv = (OPENGL_FUNC_glColor4iv) GetProcAddress(opengl_module, "glColor4iv");
	if (opengl_context.glColor4iv == NULL)
		return false;

	opengl_context.glColor4s = (OPENGL_FUNC_glColor4s) GetProcAddress(opengl_module, "glColor4s");
	if (opengl_context.glColor4s == NULL)
		return false;

	opengl_context.glColor4sv = (OPENGL_FUNC_glColor4sv) GetProcAddress(opengl_module, "glColor4sv");
	if (opengl_context.glColor4sv == NULL)
		return false;

	opengl_context.glColor4ub = (OPENGL_FUNC_glColor4ub) GetProcAddress(opengl_module, "glColor4ub");
	if (opengl_context.glColor4ub == NULL)
		return false;

	opengl_context.glColor4ubv = (OPENGL_FUNC_glColor4ubv) GetProcAddress(opengl_module, "glColor4ubv");
	if (opengl_context.glColor4ubv == NULL)
		return false;

	opengl_context.glColor4ui = (OPENGL_FUNC_glColor4ui) GetProcAddress(opengl_module, "glColor4ui");
	if (opengl_context.glColor4ui == NULL)
		return false;

	opengl_context.glColor4uiv = (OPENGL_FUNC_glColor4uiv) GetProcAddress(opengl_module, "glColor4uiv");
	if (opengl_context.glColor4uiv == NULL)
		return false;

	opengl_context.glColor4us = (OPENGL_FUNC_glColor4us) GetProcAddress(opengl_module, "glColor4us");
	if (opengl_context.glColor4us == NULL)
		return false;

	opengl_context.glColor4usv = (OPENGL_FUNC_glColor4usv) GetProcAddress(opengl_module, "glColor4usv");
	if (opengl_context.glColor4usv == NULL)
		return false;

	opengl_context.glColorMask = (OPENGL_FUNC_glColorMask) GetProcAddress(opengl_module, "glColorMask");
	if (opengl_context.glColorMask == NULL)
		return false;

	opengl_context.glColorMaterial = (OPENGL_FUNC_glColorMaterial) GetProcAddress(opengl_module, "glColorMaterial");
	if (opengl_context.glColorMaterial == NULL)
		return false;

	opengl_context.glColorPointer = (OPENGL_FUNC_glColorPointer) GetProcAddress(opengl_module, "glColorPointer");
	if (opengl_context.glColorPointer == NULL)
		return false;

	opengl_context.glCopyPixels = (OPENGL_FUNC_glCopyPixels) GetProcAddress(opengl_module, "glCopyPixels");
	if (opengl_context.glCopyPixels == NULL)
		return false;

	opengl_context.glCopyTexImage1D = (OPENGL_FUNC_glCopyTexImage1D) GetProcAddress(opengl_module, "glCopyTexImage1D");
	if (opengl_context.glCopyTexImage1D == NULL)
		return false;

	opengl_context.glCopyTexImage2D = (OPENGL_FUNC_glCopyTexImage2D) GetProcAddress(opengl_module, "glCopyTexImage2D");
	if (opengl_context.glCopyTexImage2D == NULL)
		return false;

	opengl_context.glCopyTexSubImage1D = (OPENGL_FUNC_glCopyTexSubImage1D) GetProcAddress(opengl_module, "glCopyTexSubImage1D");
	if (opengl_context.glCopyTexSubImage1D == NULL)
		return false;

	opengl_context.glCopyTexSubImage2D = (OPENGL_FUNC_glCopyTexSubImage2D) GetProcAddress(opengl_module, "glCopyTexSubImage2D");
	if (opengl_context.glCopyTexSubImage2D == NULL)
		return false;

	opengl_context.glCullFace = (OPENGL_FUNC_glCullFace) GetProcAddress(opengl_module, "glCullFace");
	if (opengl_context.glCullFace == NULL)
		return false;

	opengl_context.glDeleteLists = (OPENGL_FUNC_glDeleteLists) GetProcAddress(opengl_module, "glDeleteLists");
	if (opengl_context.glDeleteLists == NULL)
		return false;

	opengl_context.glDeleteTextures = (OPENGL_FUNC_glDeleteTextures) GetProcAddress(opengl_module, "glDeleteTextures");
	if (opengl_context.glDeleteTextures == NULL)
		return false;

	opengl_context.glDepthFunc = (OPENGL_FUNC_glDepthFunc) GetProcAddress(opengl_module, "glDepthFunc");
	if (opengl_context.glDepthFunc == NULL)
		return false;

	opengl_context.glDepthMask = (OPENGL_FUNC_glDepthMask) GetProcAddress(opengl_module, "glDepthMask");
	if (opengl_context.glDepthMask == NULL)
		return false;

	opengl_context.glDepthRange = (OPENGL_FUNC_glDepthRange) GetProcAddress(opengl_module, "glDepthRange");
	if (opengl_context.glDepthRange == NULL)
		return false;

	opengl_context.glDisable = (OPENGL_FUNC_glDisable) GetProcAddress(opengl_module, "glDisable");
	if (opengl_context.glDisable == NULL)
		return false;

	opengl_context.glDisableClientState = (OPENGL_FUNC_glDisableClientState) GetProcAddress(opengl_module, "glDisableClientState");
	if (opengl_context.glDisableClientState == NULL)
		return false;

	opengl_context.glDrawArrays = (OPENGL_FUNC_glDrawArrays) GetProcAddress(opengl_module, "glDrawArrays");
	if (opengl_context.glDrawArrays == NULL)
		return false;

	opengl_context.glDrawBuffer = (OPENGL_FUNC_glDrawBuffer) GetProcAddress(opengl_module, "glDrawBuffer");
	if (opengl_context.glDrawBuffer == NULL)
		return false;

	opengl_context.glDrawElements = (OPENGL_FUNC_glDrawElements) GetProcAddress(opengl_module, "glDrawElements");
	if (opengl_context.glDrawElements == NULL)
		return false;

	opengl_context.glDrawPixels = (OPENGL_FUNC_glDrawPixels) GetProcAddress(opengl_module, "glDrawPixels");
	if (opengl_context.glDrawPixels == NULL)
		return false;

	opengl_context.glEdgeFlag = (OPENGL_FUNC_glEdgeFlag) GetProcAddress(opengl_module, "glEdgeFlag");
	if (opengl_context.glEdgeFlag == NULL)
		return false;

	opengl_context.glEdgeFlagPointer = (OPENGL_FUNC_glEdgeFlagPointer) GetProcAddress(opengl_module, "glEdgeFlagPointer");
	if (opengl_context.glEdgeFlagPointer == NULL)
		return false;

	opengl_context.glEdgeFlagv = (OPENGL_FUNC_glEdgeFlagv) GetProcAddress(opengl_module, "glEdgeFlagv");
	if (opengl_context.glEdgeFlagv == NULL)
		return false;

	opengl_context.glEnable = (OPENGL_FUNC_glEnable) GetProcAddress(opengl_module, "glEnable");
	if (opengl_context.glEnable == NULL)
		return false;

	opengl_context.glEnableClientState = (OPENGL_FUNC_glEnableClientState) GetProcAddress(opengl_module, "glEnableClientState");
	if (opengl_context.glEnableClientState == NULL)
		return false;

	opengl_context.glEnd = (OPENGL_FUNC_glEnd) GetProcAddress(opengl_module, "glEnd");
	if (opengl_context.glEnd == NULL)
		return false;

	opengl_context.glEndList = (OPENGL_FUNC_glEndList) GetProcAddress(opengl_module, "glEndList");
	if (opengl_context.glEndList == NULL)
		return false;

	opengl_context.glEvalCoord1d = (OPENGL_FUNC_glEvalCoord1d) GetProcAddress(opengl_module, "glEvalCoord1d");
	if (opengl_context.glEvalCoord1d == NULL)
		return false;

	opengl_context.glEvalCoord1dv = (OPENGL_FUNC_glEvalCoord1dv) GetProcAddress(opengl_module, "glEvalCoord1dv");
	if (opengl_context.glEvalCoord1dv == NULL)
		return false;

	opengl_context.glEvalCoord1f = (OPENGL_FUNC_glEvalCoord1f) GetProcAddress(opengl_module, "glEvalCoord1f");
	if (opengl_context.glEvalCoord1f == NULL)
		return false;

	opengl_context.glEvalCoord1fv = (OPENGL_FUNC_glEvalCoord1fv) GetProcAddress(opengl_module, "glEvalCoord1fv");
	if (opengl_context.glEvalCoord1fv == NULL)
		return false;

	opengl_context.glEvalCoord2d = (OPENGL_FUNC_glEvalCoord2d) GetProcAddress(opengl_module, "glEvalCoord2d");
	if (opengl_context.glEvalCoord2d == NULL)
		return false;

	opengl_context.glEvalCoord2dv = (OPENGL_FUNC_glEvalCoord2dv) GetProcAddress(opengl_module, "glEvalCoord2dv");
	if (opengl_context.glEvalCoord2dv == NULL)
		return false;

	opengl_context.glEvalCoord2f = (OPENGL_FUNC_glEvalCoord2f) GetProcAddress(opengl_module, "glEvalCoord2f");
	if (opengl_context.glEvalCoord2f == NULL)
		return false;

	opengl_context.glEvalCoord2fv = (OPENGL_FUNC_glEvalCoord2fv) GetProcAddress(opengl_module, "glEvalCoord2fv");
	if (opengl_context.glEvalCoord2fv == NULL)
		return false;

	opengl_context.glEvalMesh1 = (OPENGL_FUNC_glEvalMesh1) GetProcAddress(opengl_module, "glEvalMesh1");
	if (opengl_context.glEvalMesh1 == NULL)
		return false;

	opengl_context.glEvalMesh2 = (OPENGL_FUNC_glEvalMesh2) GetProcAddress(opengl_module, "glEvalMesh2");
	if (opengl_context.glEvalMesh2 == NULL)
		return false;

	opengl_context.glEvalPoint1 = (OPENGL_FUNC_glEvalPoint1) GetProcAddress(opengl_module, "glEvalPoint1");
	if (opengl_context.glEvalPoint1 == NULL)
		return false;

	opengl_context.glEvalPoint2 = (OPENGL_FUNC_glEvalPoint2) GetProcAddress(opengl_module, "glEvalPoint2");
	if (opengl_context.glEvalPoint2 == NULL)
		return false;

	opengl_context.glFeedbackBuffer = (OPENGL_FUNC_glFeedbackBuffer) GetProcAddress(opengl_module, "glFeedbackBuffer");
	if (opengl_context.glFeedbackBuffer == NULL)
		return false;

	opengl_context.glFinish = (OPENGL_FUNC_glFinish) GetProcAddress(opengl_module, "glFinish");
	if (opengl_context.glFinish == NULL)
		return false;

	opengl_context.glFlush = (OPENGL_FUNC_glFlush) GetProcAddress(opengl_module, "glFlush");
	if (opengl_context.glFlush == NULL)
		return false;

	opengl_context.glFogf = (OPENGL_FUNC_glFogf) GetProcAddress(opengl_module, "glFogf");
	if (opengl_context.glFogf == NULL)
		return false;

	opengl_context.glFogfv = (OPENGL_FUNC_glFogfv) GetProcAddress(opengl_module, "glFogfv");
	if (opengl_context.glFogfv == NULL)
		return false;

	opengl_context.glFogi = (OPENGL_FUNC_glFogi) GetProcAddress(opengl_module, "glFogi");
	if (opengl_context.glFogi == NULL)
		return false;

	opengl_context.glFogiv = (OPENGL_FUNC_glFogiv) GetProcAddress(opengl_module, "glFogiv");
	if (opengl_context.glFogiv == NULL)
		return false;

	opengl_context.glFrontFace = (OPENGL_FUNC_glFrontFace) GetProcAddress(opengl_module, "glFrontFace");
	if (opengl_context.glFrontFace == NULL)
		return false;

	opengl_context.glFrustum = (OPENGL_FUNC_glFrustum) GetProcAddress(opengl_module, "glFrustum");
	if (opengl_context.glFrustum == NULL)
		return false;

	opengl_context.glGenLists = (OPENGL_FUNC_glGenLists) GetProcAddress(opengl_module, "glGenLists");
	if (opengl_context.glGenLists == NULL)
		return false;

	opengl_context.glGenTextures = (OPENGL_FUNC_glGenTextures) GetProcAddress(opengl_module, "glGenTextures");
	if (opengl_context.glGenTextures == NULL)
		return false;

	opengl_context.glGetBooleanv = (OPENGL_FUNC_glGetBooleanv) GetProcAddress(opengl_module, "glGetBooleanv");
	if (opengl_context.glGetBooleanv == NULL)
		return false;

	opengl_context.glGetClipPlane = (OPENGL_FUNC_glGetClipPlane) GetProcAddress(opengl_module, "glGetClipPlane");
	if (opengl_context.glGetClipPlane == NULL)
		return false;

	opengl_context.glGetDoublev = (OPENGL_FUNC_glGetDoublev) GetProcAddress(opengl_module, "glGetDoublev");
	if (opengl_context.glGetDoublev == NULL)
		return false;

	opengl_context.glGetError = (OPENGL_FUNC_glGetError) GetProcAddress(opengl_module, "glGetError");
	if (opengl_context.glGetError == NULL)
		return false;

	opengl_context.glGetFloatv = (OPENGL_FUNC_glGetFloatv) GetProcAddress(opengl_module, "glGetFloatv");
	if (opengl_context.glGetFloatv == NULL)
		return false;

	opengl_context.glGetIntegerv = (OPENGL_FUNC_glGetIntegerv) GetProcAddress(opengl_module, "glGetIntegerv");
	if (opengl_context.glGetIntegerv == NULL)
		return false;

	opengl_context.glGetLightfv = (OPENGL_FUNC_glGetLightfv) GetProcAddress(opengl_module, "glGetLightfv");
	if (opengl_context.glGetLightfv == NULL)
		return false;

	opengl_context.glGetLightiv = (OPENGL_FUNC_glGetLightiv) GetProcAddress(opengl_module, "glGetLightiv");
	if (opengl_context.glGetLightiv == NULL)
		return false;

	opengl_context.glGetMapdv = (OPENGL_FUNC_glGetMapdv) GetProcAddress(opengl_module, "glGetMapdv");
	if (opengl_context.glGetMapdv == NULL)
		return false;

	opengl_context.glGetMapfv = (OPENGL_FUNC_glGetMapfv) GetProcAddress(opengl_module, "glGetMapfv");
	if (opengl_context.glGetMapfv == NULL)
		return false;

	opengl_context.glGetMapiv = (OPENGL_FUNC_glGetMapiv) GetProcAddress(opengl_module, "glGetMapiv");
	if (opengl_context.glGetMapiv == NULL)
		return false;

	opengl_context.glGetMaterialfv = (OPENGL_FUNC_glGetMaterialfv) GetProcAddress(opengl_module, "glGetMaterialfv");
	if (opengl_context.glGetMaterialfv == NULL)
		return false;

	opengl_context.glGetMaterialiv = (OPENGL_FUNC_glGetMaterialiv) GetProcAddress(opengl_module, "glGetMaterialiv");
	if (opengl_context.glGetMaterialiv == NULL)
		return false;

	opengl_context.glGetPixelMapfv = (OPENGL_FUNC_glGetPixelMapfv) GetProcAddress(opengl_module, "glGetPixelMapfv");
	if (opengl_context.glGetPixelMapfv == NULL)
		return false;

	opengl_context.glGetPixelMapuiv = (OPENGL_FUNC_glGetPixelMapuiv) GetProcAddress(opengl_module, "glGetPixelMapuiv");
	if (opengl_context.glGetPixelMapuiv == NULL)
		return false;

	opengl_context.glGetPixelMapusv = (OPENGL_FUNC_glGetPixelMapusv) GetProcAddress(opengl_module, "glGetPixelMapusv");
	if (opengl_context.glGetPixelMapusv == NULL)
		return false;

	opengl_context.glGetPointerv = (OPENGL_FUNC_glGetPointerv) GetProcAddress(opengl_module, "glGetPointerv");
	if (opengl_context.glGetPointerv == NULL)
		return false;

	opengl_context.glGetPolygonStipple = (OPENGL_FUNC_glGetPolygonStipple) GetProcAddress(opengl_module, "glGetPolygonStipple");
	if (opengl_context.glGetPolygonStipple == NULL)
		return false;

	opengl_context.glGetString = (OPENGL_FUNC_glGetString) GetProcAddress(opengl_module, "glGetString");
	if (opengl_context.glGetString == NULL)
		return false;

	opengl_context.glGetTexEnvfv = (OPENGL_FUNC_glGetTexEnvfv) GetProcAddress(opengl_module, "glGetTexEnvfv");
	if (opengl_context.glGetTexEnvfv == NULL)
		return false;

	opengl_context.glGetTexEnviv = (OPENGL_FUNC_glGetTexEnviv) GetProcAddress(opengl_module, "glGetTexEnviv");
	if (opengl_context.glGetTexEnviv == NULL)
		return false;

	opengl_context.glGetTexGendv = (OPENGL_FUNC_glGetTexGendv) GetProcAddress(opengl_module, "glGetTexGendv");
	if (opengl_context.glGetTexGendv == NULL)
		return false;

	opengl_context.glGetTexGenfv = (OPENGL_FUNC_glGetTexGenfv) GetProcAddress(opengl_module, "glGetTexGenfv");
	if (opengl_context.glGetTexGenfv == NULL)
		return false;

	opengl_context.glGetTexGeniv = (OPENGL_FUNC_glGetTexGeniv) GetProcAddress(opengl_module, "glGetTexGeniv");
	if (opengl_context.glGetTexGeniv == NULL)
		return false;

	opengl_context.glGetTexImage = (OPENGL_FUNC_glGetTexImage) GetProcAddress(opengl_module, "glGetTexImage");
	if (opengl_context.glGetTexImage == NULL)
		return false;

	opengl_context.glGetTexLevelParameterfv = (OPENGL_FUNC_glGetTexLevelParameterfv) GetProcAddress(opengl_module, "glGetTexLevelParameterfv");
	if (opengl_context.glGetTexLevelParameterfv == NULL)
		return false;

	opengl_context.glGetTexLevelParameteriv = (OPENGL_FUNC_glGetTexLevelParameteriv) GetProcAddress(opengl_module, "glGetTexLevelParameteriv");
	if (opengl_context.glGetTexLevelParameteriv == NULL)
		return false;

	opengl_context.glGetTexParameterfv = (OPENGL_FUNC_glGetTexParameterfv) GetProcAddress(opengl_module, "glGetTexParameterfv");
	if (opengl_context.glGetTexParameterfv == NULL)
		return false;

	opengl_context.glGetTexParameteriv = (OPENGL_FUNC_glGetTexParameteriv) GetProcAddress(opengl_module, "glGetTexParameteriv");
	if (opengl_context.glGetTexParameteriv == NULL)
		return false;

	opengl_context.glHint = (OPENGL_FUNC_glHint) GetProcAddress(opengl_module, "glHint");
	if (opengl_context.glHint == NULL)
		return false;

	opengl_context.glIndexMask = (OPENGL_FUNC_glIndexMask) GetProcAddress(opengl_module, "glIndexMask");
	if (opengl_context.glIndexMask == NULL)
		return false;

	opengl_context.glIndexPointer = (OPENGL_FUNC_glIndexPointer) GetProcAddress(opengl_module, "glIndexPointer");
	if (opengl_context.glIndexPointer == NULL)
		return false;

	opengl_context.glIndexd = (OPENGL_FUNC_glIndexd) GetProcAddress(opengl_module, "glIndexd");
	if (opengl_context.glIndexd == NULL)
		return false;

	opengl_context.glIndexdv = (OPENGL_FUNC_glIndexdv) GetProcAddress(opengl_module, "glIndexdv");
	if (opengl_context.glIndexdv == NULL)
		return false;

	opengl_context.glIndexf = (OPENGL_FUNC_glIndexf) GetProcAddress(opengl_module, "glIndexf");
	if (opengl_context.glIndexf == NULL)
		return false;

	opengl_context.glIndexfv = (OPENGL_FUNC_glIndexfv) GetProcAddress(opengl_module, "glIndexfv");
	if (opengl_context.glIndexfv == NULL)
		return false;

	opengl_context.glIndexi = (OPENGL_FUNC_glIndexi) GetProcAddress(opengl_module, "glIndexi");
	if (opengl_context.glIndexi == NULL)
		return false;

	opengl_context.glIndexiv = (OPENGL_FUNC_glIndexiv) GetProcAddress(opengl_module, "glIndexiv");
	if (opengl_context.glIndexiv == NULL)
		return false;

	opengl_context.glIndexs = (OPENGL_FUNC_glIndexs) GetProcAddress(opengl_module, "glIndexs");
	if (opengl_context.glIndexs == NULL)
		return false;

	opengl_context.glIndexsv = (OPENGL_FUNC_glIndexsv) GetProcAddress(opengl_module, "glIndexsv");
	if (opengl_context.glIndexsv == NULL)
		return false;

	opengl_context.glIndexub = (OPENGL_FUNC_glIndexub) GetProcAddress(opengl_module, "glIndexub");
	if (opengl_context.glIndexub == NULL)
		return false;

	opengl_context.glIndexubv = (OPENGL_FUNC_glIndexubv) GetProcAddress(opengl_module, "glIndexubv");
	if (opengl_context.glIndexubv == NULL)
		return false;

	opengl_context.glInitNames = (OPENGL_FUNC_glInitNames) GetProcAddress(opengl_module, "glInitNames");
	if (opengl_context.glInitNames == NULL)
		return false;

	opengl_context.glInterleavedArrays = (OPENGL_FUNC_glInterleavedArrays) GetProcAddress(opengl_module, "glInterleavedArrays");
	if (opengl_context.glInterleavedArrays == NULL)
		return false;

	opengl_context.glIsEnabled = (OPENGL_FUNC_glIsEnabled) GetProcAddress(opengl_module, "glIsEnabled");
	if (opengl_context.glIsEnabled == NULL)
		return false;

	opengl_context.glIsList = (OPENGL_FUNC_glIsList) GetProcAddress(opengl_module, "glIsList");
	if (opengl_context.glIsList == NULL)
		return false;

	opengl_context.glIsTexture = (OPENGL_FUNC_glIsTexture) GetProcAddress(opengl_module, "glIsTexture");
	if (opengl_context.glIsTexture == NULL)
		return false;

	opengl_context.glLightModelf = (OPENGL_FUNC_glLightModelf) GetProcAddress(opengl_module, "glLightModelf");
	if (opengl_context.glLightModelf == NULL)
		return false;

	opengl_context.glLightModelfv = (OPENGL_FUNC_glLightModelfv) GetProcAddress(opengl_module, "glLightModelfv");
	if (opengl_context.glLightModelfv == NULL)
		return false;

	opengl_context.glLightModeli = (OPENGL_FUNC_glLightModeli) GetProcAddress(opengl_module, "glLightModeli");
	if (opengl_context.glLightModeli == NULL)
		return false;

	opengl_context.glLightModeliv = (OPENGL_FUNC_glLightModeliv) GetProcAddress(opengl_module, "glLightModeliv");
	if (opengl_context.glLightModeliv == NULL)
		return false;

	opengl_context.glLightf = (OPENGL_FUNC_glLightf) GetProcAddress(opengl_module, "glLightf");
	if (opengl_context.glLightf == NULL)
		return false;

	opengl_context.glLightfv = (OPENGL_FUNC_glLightfv) GetProcAddress(opengl_module, "glLightfv");
	if (opengl_context.glLightfv == NULL)
		return false;

	opengl_context.glLighti = (OPENGL_FUNC_glLighti) GetProcAddress(opengl_module, "glLighti");
	if (opengl_context.glLighti == NULL)
		return false;

	opengl_context.glLightiv = (OPENGL_FUNC_glLightiv) GetProcAddress(opengl_module, "glLightiv");
	if (opengl_context.glLightiv == NULL)
		return false;

	opengl_context.glLineStipple = (OPENGL_FUNC_glLineStipple) GetProcAddress(opengl_module, "glLineStipple");
	if (opengl_context.glLineStipple == NULL)
		return false;

	opengl_context.glLineWidth = (OPENGL_FUNC_glLineWidth) GetProcAddress(opengl_module, "glLineWidth");
	if (opengl_context.glLineWidth == NULL)
		return false;

	opengl_context.glListBase = (OPENGL_FUNC_glListBase) GetProcAddress(opengl_module, "glListBase");
	if (opengl_context.glListBase == NULL)
		return false;

	opengl_context.glLoadIdentity = (OPENGL_FUNC_glLoadIdentity) GetProcAddress(opengl_module, "glLoadIdentity");
	if (opengl_context.glLoadIdentity == NULL)
		return false;

	opengl_context.glLoadMatrixd = (OPENGL_FUNC_glLoadMatrixd) GetProcAddress(opengl_module, "glLoadMatrixd");
	if (opengl_context.glLoadMatrixd == NULL)
		return false;

	opengl_context.glLoadMatrixf = (OPENGL_FUNC_glLoadMatrixf) GetProcAddress(opengl_module, "glLoadMatrixf");
	if (opengl_context.glLoadMatrixf == NULL)
		return false;

	opengl_context.glLoadName = (OPENGL_FUNC_glLoadName) GetProcAddress(opengl_module, "glLoadName");
	if (opengl_context.glLoadName == NULL)
		return false;

	opengl_context.glLogicOp = (OPENGL_FUNC_glLogicOp) GetProcAddress(opengl_module, "glLogicOp");
	if (opengl_context.glLogicOp == NULL)
		return false;

	opengl_context.glMap1d = (OPENGL_FUNC_glMap1d) GetProcAddress(opengl_module, "glMap1d");
	if (opengl_context.glMap1d == NULL)
		return false;

	opengl_context.glMap1f = (OPENGL_FUNC_glMap1f) GetProcAddress(opengl_module, "glMap1f");
	if (opengl_context.glMap1f == NULL)
		return false;

	opengl_context.glMap2d = (OPENGL_FUNC_glMap2d) GetProcAddress(opengl_module, "glMap2d");
	if (opengl_context.glMap2d == NULL)
		return false;

	opengl_context.glMap2f = (OPENGL_FUNC_glMap2f) GetProcAddress(opengl_module, "glMap2f");
	if (opengl_context.glMap2f == NULL)
		return false;

	opengl_context.glMapGrid1d = (OPENGL_FUNC_glMapGrid1d) GetProcAddress(opengl_module, "glMapGrid1d");
	if (opengl_context.glMapGrid1d == NULL)
		return false;

	opengl_context.glMapGrid1f = (OPENGL_FUNC_glMapGrid1f) GetProcAddress(opengl_module, "glMapGrid1f");
	if (opengl_context.glMapGrid1f == NULL)
		return false;

	opengl_context.glMapGrid2d = (OPENGL_FUNC_glMapGrid2d) GetProcAddress(opengl_module, "glMapGrid2d");
	if (opengl_context.glMapGrid2d == NULL)
		return false;

	opengl_context.glMapGrid2f = (OPENGL_FUNC_glMapGrid2f) GetProcAddress(opengl_module, "glMapGrid2f");
	if (opengl_context.glMapGrid2f == NULL)
		return false;

	opengl_context.glMaterialf = (OPENGL_FUNC_glMaterialf) GetProcAddress(opengl_module, "glMaterialf");
	if (opengl_context.glMaterialf == NULL)
		return false;

	opengl_context.glMaterialfv = (OPENGL_FUNC_glMaterialfv) GetProcAddress(opengl_module, "glMaterialfv");
	if (opengl_context.glMaterialfv == NULL)
		return false;

	opengl_context.glMateriali = (OPENGL_FUNC_glMateriali) GetProcAddress(opengl_module, "glMateriali");
	if (opengl_context.glMateriali == NULL)
		return false;

	opengl_context.glMaterialiv = (OPENGL_FUNC_glMaterialiv) GetProcAddress(opengl_module, "glMaterialiv");
	if (opengl_context.glMaterialiv == NULL)
		return false;

	opengl_context.glMatrixMode = (OPENGL_FUNC_glMatrixMode) GetProcAddress(opengl_module, "glMatrixMode");
	if (opengl_context.glMatrixMode == NULL)
		return false;

	opengl_context.glMultMatrixd = (OPENGL_FUNC_glMultMatrixd) GetProcAddress(opengl_module, "glMultMatrixd");
	if (opengl_context.glMultMatrixd == NULL)
		return false;

	opengl_context.glMultMatrixf = (OPENGL_FUNC_glMultMatrixf) GetProcAddress(opengl_module, "glMultMatrixf");
	if (opengl_context.glMultMatrixf == NULL)
		return false;

	opengl_context.glNewList = (OPENGL_FUNC_glNewList) GetProcAddress(opengl_module, "glNewList");
	if (opengl_context.glNewList == NULL)
		return false;

	opengl_context.glNormal3b = (OPENGL_FUNC_glNormal3b) GetProcAddress(opengl_module, "glNormal3b");
	if (opengl_context.glNormal3b == NULL)
		return false;

	opengl_context.glNormal3bv = (OPENGL_FUNC_glNormal3bv) GetProcAddress(opengl_module, "glNormal3bv");
	if (opengl_context.glNormal3bv == NULL)
		return false;

	opengl_context.glNormal3d = (OPENGL_FUNC_glNormal3d) GetProcAddress(opengl_module, "glNormal3d");
	if (opengl_context.glNormal3d == NULL)
		return false;

	opengl_context.glNormal3dv = (OPENGL_FUNC_glNormal3dv) GetProcAddress(opengl_module, "glNormal3dv");
	if (opengl_context.glNormal3dv == NULL)
		return false;

	opengl_context.glNormal3f = (OPENGL_FUNC_glNormal3f) GetProcAddress(opengl_module, "glNormal3f");
	if (opengl_context.glNormal3f == NULL)
		return false;

	opengl_context.glNormal3fv = (OPENGL_FUNC_glNormal3fv) GetProcAddress(opengl_module, "glNormal3fv");
	if (opengl_context.glNormal3fv == NULL)
		return false;

	opengl_context.glNormal3i = (OPENGL_FUNC_glNormal3i) GetProcAddress(opengl_module, "glNormal3i");
	if (opengl_context.glNormal3i == NULL)
		return false;

	opengl_context.glNormal3iv = (OPENGL_FUNC_glNormal3iv) GetProcAddress(opengl_module, "glNormal3iv");
	if (opengl_context.glNormal3iv == NULL)
		return false;

	opengl_context.glNormal3s = (OPENGL_FUNC_glNormal3s) GetProcAddress(opengl_module, "glNormal3s");
	if (opengl_context.glNormal3s == NULL)
		return false;

	opengl_context.glNormal3sv = (OPENGL_FUNC_glNormal3sv) GetProcAddress(opengl_module, "glNormal3sv");
	if (opengl_context.glNormal3sv == NULL)
		return false;

	opengl_context.glNormalPointer = (OPENGL_FUNC_glNormalPointer) GetProcAddress(opengl_module, "glNormalPointer");
	if (opengl_context.glNormalPointer == NULL)
		return false;

	opengl_context.glOrtho = (OPENGL_FUNC_glOrtho) GetProcAddress(opengl_module, "glOrtho");
	if (opengl_context.glOrtho == NULL)
		return false;

	opengl_context.glPassThrough = (OPENGL_FUNC_glPassThrough) GetProcAddress(opengl_module, "glPassThrough");
	if (opengl_context.glPassThrough == NULL)
		return false;

	opengl_context.glPixelMapfv = (OPENGL_FUNC_glPixelMapfv) GetProcAddress(opengl_module, "glPixelMapfv");
	if (opengl_context.glPixelMapfv == NULL)
		return false;

	opengl_context.glPixelMapuiv = (OPENGL_FUNC_glPixelMapuiv) GetProcAddress(opengl_module, "glPixelMapuiv");
	if (opengl_context.glPixelMapuiv == NULL)
		return false;

	opengl_context.glPixelMapusv = (OPENGL_FUNC_glPixelMapusv) GetProcAddress(opengl_module, "glPixelMapusv");
	if (opengl_context.glPixelMapusv == NULL)
		return false;

	opengl_context.glPixelStoref = (OPENGL_FUNC_glPixelStoref) GetProcAddress(opengl_module, "glPixelStoref");
	if (opengl_context.glPixelStoref == NULL)
		return false;

	opengl_context.glPixelStorei = (OPENGL_FUNC_glPixelStorei) GetProcAddress(opengl_module, "glPixelStorei");
	if (opengl_context.glPixelStorei == NULL)
		return false;

	opengl_context.glPixelTransferf = (OPENGL_FUNC_glPixelTransferf) GetProcAddress(opengl_module, "glPixelTransferf");
	if (opengl_context.glPixelTransferf == NULL)
		return false;

	opengl_context.glPixelTransferi = (OPENGL_FUNC_glPixelTransferi) GetProcAddress(opengl_module, "glPixelTransferi");
	if (opengl_context.glPixelTransferi == NULL)
		return false;

	opengl_context.glPixelZoom = (OPENGL_FUNC_glPixelZoom) GetProcAddress(opengl_module, "glPixelZoom");
	if (opengl_context.glPixelZoom == NULL)
		return false;

	opengl_context.glPointSize = (OPENGL_FUNC_glPointSize) GetProcAddress(opengl_module, "glPointSize");
	if (opengl_context.glPointSize == NULL)
		return false;

	opengl_context.glPolygonMode = (OPENGL_FUNC_glPolygonMode) GetProcAddress(opengl_module, "glPolygonMode");
	if (opengl_context.glPolygonMode == NULL)
		return false;

	opengl_context.glPolygonOffset = (OPENGL_FUNC_glPolygonOffset) GetProcAddress(opengl_module, "glPolygonOffset");
	if (opengl_context.glPolygonOffset == NULL)
		return false;

	opengl_context.glPolygonStipple = (OPENGL_FUNC_glPolygonStipple) GetProcAddress(opengl_module, "glPolygonStipple");
	if (opengl_context.glPolygonStipple == NULL)
		return false;

	opengl_context.glPopAttrib = (OPENGL_FUNC_glPopAttrib) GetProcAddress(opengl_module, "glPopAttrib");
	if (opengl_context.glPopAttrib == NULL)
		return false;

	opengl_context.glPopClientAttrib = (OPENGL_FUNC_glPopClientAttrib) GetProcAddress(opengl_module, "glPopClientAttrib");
	if (opengl_context.glPopClientAttrib == NULL)
		return false;

	opengl_context.glPopMatrix = (OPENGL_FUNC_glPopMatrix) GetProcAddress(opengl_module, "glPopMatrix");
	if (opengl_context.glPopMatrix == NULL)
		return false;

	opengl_context.glPopName = (OPENGL_FUNC_glPopName) GetProcAddress(opengl_module, "glPopName");
	if (opengl_context.glPopName == NULL)
		return false;

	opengl_context.glPrioritizeTextures = (OPENGL_FUNC_glPrioritizeTextures) GetProcAddress(opengl_module, "glPrioritizeTextures");
	if (opengl_context.glPrioritizeTextures == NULL)
		return false;

	opengl_context.glPushAttrib = (OPENGL_FUNC_glPushAttrib) GetProcAddress(opengl_module, "glPushAttrib");
	if (opengl_context.glPushAttrib == NULL)
		return false;

	opengl_context.glPushClientAttrib = (OPENGL_FUNC_glPushClientAttrib) GetProcAddress(opengl_module, "glPushClientAttrib");
	if (opengl_context.glPushClientAttrib == NULL)
		return false;

	opengl_context.glPushMatrix = (OPENGL_FUNC_glPushMatrix) GetProcAddress(opengl_module, "glPushMatrix");
	if (opengl_context.glPushMatrix == NULL)
		return false;

	opengl_context.glPushName = (OPENGL_FUNC_glPushName) GetProcAddress(opengl_module, "glPushName");
	if (opengl_context.glPushName == NULL)
		return false;

	opengl_context.glRasterPos2d = (OPENGL_FUNC_glRasterPos2d) GetProcAddress(opengl_module, "glRasterPos2d");
	if (opengl_context.glRasterPos2d == NULL)
		return false;

	opengl_context.glRasterPos2dv = (OPENGL_FUNC_glRasterPos2dv) GetProcAddress(opengl_module, "glRasterPos2dv");
	if (opengl_context.glRasterPos2dv == NULL)
		return false;

	opengl_context.glRasterPos2f = (OPENGL_FUNC_glRasterPos2f) GetProcAddress(opengl_module, "glRasterPos2f");
	if (opengl_context.glRasterPos2f == NULL)
		return false;

	opengl_context.glRasterPos2fv = (OPENGL_FUNC_glRasterPos2fv) GetProcAddress(opengl_module, "glRasterPos2fv");
	if (opengl_context.glRasterPos2fv == NULL)
		return false;

	opengl_context.glRasterPos2i = (OPENGL_FUNC_glRasterPos2i) GetProcAddress(opengl_module, "glRasterPos2i");
	if (opengl_context.glRasterPos2i == NULL)
		return false;

	opengl_context.glRasterPos2iv = (OPENGL_FUNC_glRasterPos2iv) GetProcAddress(opengl_module, "glRasterPos2iv");
	if (opengl_context.glRasterPos2iv == NULL)
		return false;

	opengl_context.glRasterPos2s = (OPENGL_FUNC_glRasterPos2s) GetProcAddress(opengl_module, "glRasterPos2s");
	if (opengl_context.glRasterPos2s == NULL)
		return false;

	opengl_context.glRasterPos2sv = (OPENGL_FUNC_glRasterPos2sv) GetProcAddress(opengl_module, "glRasterPos2sv");
	if (opengl_context.glRasterPos2sv == NULL)
		return false;

	opengl_context.glRasterPos3d = (OPENGL_FUNC_glRasterPos3d) GetProcAddress(opengl_module, "glRasterPos3d");
	if (opengl_context.glRasterPos3d == NULL)
		return false;

	opengl_context.glRasterPos3dv = (OPENGL_FUNC_glRasterPos3dv) GetProcAddress(opengl_module, "glRasterPos3dv");
	if (opengl_context.glRasterPos3dv == NULL)
		return false;

	opengl_context.glRasterPos3f = (OPENGL_FUNC_glRasterPos3f) GetProcAddress(opengl_module, "glRasterPos3f");
	if (opengl_context.glRasterPos3f == NULL)
		return false;

	opengl_context.glRasterPos3fv = (OPENGL_FUNC_glRasterPos3fv) GetProcAddress(opengl_module, "glRasterPos3fv");
	if (opengl_context.glRasterPos3fv == NULL)
		return false;

	opengl_context.glRasterPos3i = (OPENGL_FUNC_glRasterPos3i) GetProcAddress(opengl_module, "glRasterPos3i");
	if (opengl_context.glRasterPos3i == NULL)
		return false;

	opengl_context.glRasterPos3iv = (OPENGL_FUNC_glRasterPos3iv) GetProcAddress(opengl_module, "glRasterPos3iv");
	if (opengl_context.glRasterPos3iv == NULL)
		return false;

	opengl_context.glRasterPos3s = (OPENGL_FUNC_glRasterPos3s) GetProcAddress(opengl_module, "glRasterPos3s");
	if (opengl_context.glRasterPos3s == NULL)
		return false;

	opengl_context.glRasterPos3sv = (OPENGL_FUNC_glRasterPos3sv) GetProcAddress(opengl_module, "glRasterPos3sv");
	if (opengl_context.glRasterPos3sv == NULL)
		return false;

	opengl_context.glRasterPos4d = (OPENGL_FUNC_glRasterPos4d) GetProcAddress(opengl_module, "glRasterPos4d");
	if (opengl_context.glRasterPos4d == NULL)
		return false;

	opengl_context.glRasterPos4dv = (OPENGL_FUNC_glRasterPos4dv) GetProcAddress(opengl_module, "glRasterPos4dv");
	if (opengl_context.glRasterPos4dv == NULL)
		return false;

	opengl_context.glRasterPos4f = (OPENGL_FUNC_glRasterPos4f) GetProcAddress(opengl_module, "glRasterPos4f");
	if (opengl_context.glRasterPos4f == NULL)
		return false;

	opengl_context.glRasterPos4fv = (OPENGL_FUNC_glRasterPos4fv) GetProcAddress(opengl_module, "glRasterPos4fv");
	if (opengl_context.glRasterPos4fv == NULL)
		return false;

	opengl_context.glRasterPos4i = (OPENGL_FUNC_glRasterPos4i) GetProcAddress(opengl_module, "glRasterPos4i");
	if (opengl_context.glRasterPos4i == NULL)
		return false;

	opengl_context.glRasterPos4iv = (OPENGL_FUNC_glRasterPos4iv) GetProcAddress(opengl_module, "glRasterPos4iv");
	if (opengl_context.glRasterPos4iv == NULL)
		return false;

	opengl_context.glRasterPos4s = (OPENGL_FUNC_glRasterPos4s) GetProcAddress(opengl_module, "glRasterPos4s");
	if (opengl_context.glRasterPos4s == NULL)
		return false;

	opengl_context.glRasterPos4sv = (OPENGL_FUNC_glRasterPos4sv) GetProcAddress(opengl_module, "glRasterPos4sv");
	if (opengl_context.glRasterPos4sv == NULL)
		return false;

	opengl_context.glReadBuffer = (OPENGL_FUNC_glReadBuffer) GetProcAddress(opengl_module, "glReadBuffer");
	if (opengl_context.glReadBuffer == NULL)
		return false;

	opengl_context.glReadPixels = (OPENGL_FUNC_glReadPixels) GetProcAddress(opengl_module, "glReadPixels");
	if (opengl_context.glReadPixels == NULL)
		return false;

	opengl_context.glRectd = (OPENGL_FUNC_glRectd) GetProcAddress(opengl_module, "glRectd");
	if (opengl_context.glRectd == NULL)
		return false;

	opengl_context.glRectdv = (OPENGL_FUNC_glRectdv) GetProcAddress(opengl_module, "glRectdv");
	if (opengl_context.glRectdv == NULL)
		return false;

	opengl_context.glRectf = (OPENGL_FUNC_glRectf) GetProcAddress(opengl_module, "glRectf");
	if (opengl_context.glRectf == NULL)
		return false;

	opengl_context.glRectfv = (OPENGL_FUNC_glRectfv) GetProcAddress(opengl_module, "glRectfv");
	if (opengl_context.glRectfv == NULL)
		return false;

	opengl_context.glRecti = (OPENGL_FUNC_glRecti) GetProcAddress(opengl_module, "glRecti");
	if (opengl_context.glRecti == NULL)
		return false;

	opengl_context.glRectiv = (OPENGL_FUNC_glRectiv) GetProcAddress(opengl_module, "glRectiv");
	if (opengl_context.glRectiv == NULL)
		return false;

	opengl_context.glRects = (OPENGL_FUNC_glRects) GetProcAddress(opengl_module, "glRects");
	if (opengl_context.glRects == NULL)
		return false;

	opengl_context.glRectsv = (OPENGL_FUNC_glRectsv) GetProcAddress(opengl_module, "glRectsv");
	if (opengl_context.glRectsv == NULL)
		return false;

	opengl_context.glRenderMode = (OPENGL_FUNC_glRenderMode) GetProcAddress(opengl_module, "glRenderMode");
	if (opengl_context.glRenderMode == NULL)
		return false;

	opengl_context.glRotated = (OPENGL_FUNC_glRotated) GetProcAddress(opengl_module, "glRotated");
	if (opengl_context.glRotated == NULL)
		return false;

	opengl_context.glRotatef = (OPENGL_FUNC_glRotatef) GetProcAddress(opengl_module, "glRotatef");
	if (opengl_context.glRotatef == NULL)
		return false;

	opengl_context.glScaled = (OPENGL_FUNC_glScaled) GetProcAddress(opengl_module, "glScaled");
	if (opengl_context.glScaled == NULL)
		return false;

	opengl_context.glScalef = (OPENGL_FUNC_glScalef) GetProcAddress(opengl_module, "glScalef");
	if (opengl_context.glScalef == NULL)
		return false;

	opengl_context.glScissor = (OPENGL_FUNC_glScissor) GetProcAddress(opengl_module, "glScissor");
	if (opengl_context.glScissor == NULL)
		return false;

	opengl_context.glSelectBuffer = (OPENGL_FUNC_glSelectBuffer) GetProcAddress(opengl_module, "glSelectBuffer");
	if (opengl_context.glSelectBuffer == NULL)
		return false;

	opengl_context.glShadeModel = (OPENGL_FUNC_glShadeModel) GetProcAddress(opengl_module, "glShadeModel");
	if (opengl_context.glShadeModel == NULL)
		return false;

	opengl_context.glStencilFunc = (OPENGL_FUNC_glStencilFunc) GetProcAddress(opengl_module, "glStencilFunc");
	if (opengl_context.glStencilFunc == NULL)
		return false;

	opengl_context.glStencilMask = (OPENGL_FUNC_glStencilMask) GetProcAddress(opengl_module, "glStencilMask");
	if (opengl_context.glStencilMask == NULL)
		return false;

	opengl_context.glStencilOp = (OPENGL_FUNC_glStencilOp) GetProcAddress(opengl_module, "glStencilOp");
	if (opengl_context.glStencilOp == NULL)
		return false;

	opengl_context.glTexCoord1d = (OPENGL_FUNC_glTexCoord1d) GetProcAddress(opengl_module, "glTexCoord1d");
	if (opengl_context.glTexCoord1d == NULL)
		return false;

	opengl_context.glTexCoord1dv = (OPENGL_FUNC_glTexCoord1dv) GetProcAddress(opengl_module, "glTexCoord1dv");
	if (opengl_context.glTexCoord1dv == NULL)
		return false;

	opengl_context.glTexCoord1f = (OPENGL_FUNC_glTexCoord1f) GetProcAddress(opengl_module, "glTexCoord1f");
	if (opengl_context.glTexCoord1f == NULL)
		return false;

	opengl_context.glTexCoord1fv = (OPENGL_FUNC_glTexCoord1fv) GetProcAddress(opengl_module, "glTexCoord1fv");
	if (opengl_context.glTexCoord1fv == NULL)
		return false;

	opengl_context.glTexCoord1i = (OPENGL_FUNC_glTexCoord1i) GetProcAddress(opengl_module, "glTexCoord1i");
	if (opengl_context.glTexCoord1i == NULL)
		return false;

	opengl_context.glTexCoord1iv = (OPENGL_FUNC_glTexCoord1iv) GetProcAddress(opengl_module, "glTexCoord1iv");
	if (opengl_context.glTexCoord1iv == NULL)
		return false;

	opengl_context.glTexCoord1s = (OPENGL_FUNC_glTexCoord1s) GetProcAddress(opengl_module, "glTexCoord1s");
	if (opengl_context.glTexCoord1s == NULL)
		return false;

	opengl_context.glTexCoord1sv = (OPENGL_FUNC_glTexCoord1sv) GetProcAddress(opengl_module, "glTexCoord1sv");
	if (opengl_context.glTexCoord1sv == NULL)
		return false;

	opengl_context.glTexCoord2d = (OPENGL_FUNC_glTexCoord2d) GetProcAddress(opengl_module, "glTexCoord2d");
	if (opengl_context.glTexCoord2d == NULL)
		return false;

	opengl_context.glTexCoord2dv = (OPENGL_FUNC_glTexCoord2dv) GetProcAddress(opengl_module, "glTexCoord2dv");
	if (opengl_context.glTexCoord2dv == NULL)
		return false;

	opengl_context.glTexCoord2f = (OPENGL_FUNC_glTexCoord2f) GetProcAddress(opengl_module, "glTexCoord2f");
	if (opengl_context.glTexCoord2f == NULL)
		return false;

	opengl_context.glTexCoord2fv = (OPENGL_FUNC_glTexCoord2fv) GetProcAddress(opengl_module, "glTexCoord2fv");
	if (opengl_context.glTexCoord2fv == NULL)
		return false;

	opengl_context.glTexCoord2i = (OPENGL_FUNC_glTexCoord2i) GetProcAddress(opengl_module, "glTexCoord2i");
	if (opengl_context.glTexCoord2i == NULL)
		return false;

	opengl_context.glTexCoord2iv = (OPENGL_FUNC_glTexCoord2iv) GetProcAddress(opengl_module, "glTexCoord2iv");
	if (opengl_context.glTexCoord2iv == NULL)
		return false;

	opengl_context.glTexCoord2s = (OPENGL_FUNC_glTexCoord2s) GetProcAddress(opengl_module, "glTexCoord2s");
	if (opengl_context.glTexCoord2s == NULL)
		return false;

	opengl_context.glTexCoord2sv = (OPENGL_FUNC_glTexCoord2sv) GetProcAddress(opengl_module, "glTexCoord2sv");
	if (opengl_context.glTexCoord2sv == NULL)
		return false;

	opengl_context.glTexCoord3d = (OPENGL_FUNC_glTexCoord3d) GetProcAddress(opengl_module, "glTexCoord3d");
	if (opengl_context.glTexCoord3d == NULL)
		return false;

	opengl_context.glTexCoord3dv = (OPENGL_FUNC_glTexCoord3dv) GetProcAddress(opengl_module, "glTexCoord3dv");
	if (opengl_context.glTexCoord3dv == NULL)
		return false;

	opengl_context.glTexCoord3f = (OPENGL_FUNC_glTexCoord3f) GetProcAddress(opengl_module, "glTexCoord3f");
	if (opengl_context.glTexCoord3f == NULL)
		return false;

	opengl_context.glTexCoord3fv = (OPENGL_FUNC_glTexCoord3fv) GetProcAddress(opengl_module, "glTexCoord3fv");
	if (opengl_context.glTexCoord3fv == NULL)
		return false;

	opengl_context.glTexCoord3i = (OPENGL_FUNC_glTexCoord3i) GetProcAddress(opengl_module, "glTexCoord3i");
	if (opengl_context.glTexCoord3i == NULL)
		return false;

	opengl_context.glTexCoord3iv = (OPENGL_FUNC_glTexCoord3iv) GetProcAddress(opengl_module, "glTexCoord3iv");
	if (opengl_context.glTexCoord3iv == NULL)
		return false;

	opengl_context.glTexCoord3s = (OPENGL_FUNC_glTexCoord3s) GetProcAddress(opengl_module, "glTexCoord3s");
	if (opengl_context.glTexCoord3s == NULL)
		return false;

	opengl_context.glTexCoord3sv = (OPENGL_FUNC_glTexCoord3sv) GetProcAddress(opengl_module, "glTexCoord3sv");
	if (opengl_context.glTexCoord3sv == NULL)
		return false;

	opengl_context.glTexCoord4d = (OPENGL_FUNC_glTexCoord4d) GetProcAddress(opengl_module, "glTexCoord4d");
	if (opengl_context.glTexCoord4d == NULL)
		return false;

	opengl_context.glTexCoord4dv = (OPENGL_FUNC_glTexCoord4dv) GetProcAddress(opengl_module, "glTexCoord4dv");
	if (opengl_context.glTexCoord4dv == NULL)
		return false;

	opengl_context.glTexCoord4f = (OPENGL_FUNC_glTexCoord4f) GetProcAddress(opengl_module, "glTexCoord4f");
	if (opengl_context.glTexCoord4f == NULL)
		return false;

	opengl_context.glTexCoord4fv = (OPENGL_FUNC_glTexCoord4fv) GetProcAddress(opengl_module, "glTexCoord4fv");
	if (opengl_context.glTexCoord4fv == NULL)
		return false;

	opengl_context.glTexCoord4i = (OPENGL_FUNC_glTexCoord4i) GetProcAddress(opengl_module, "glTexCoord4i");
	if (opengl_context.glTexCoord4i == NULL)
		return false;

	opengl_context.glTexCoord4iv = (OPENGL_FUNC_glTexCoord4iv) GetProcAddress(opengl_module, "glTexCoord4iv");
	if (opengl_context.glTexCoord4iv == NULL)
		return false;

	opengl_context.glTexCoord4s = (OPENGL_FUNC_glTexCoord4s) GetProcAddress(opengl_module, "glTexCoord4s");
	if (opengl_context.glTexCoord4s == NULL)
		return false;

	opengl_context.glTexCoord4sv = (OPENGL_FUNC_glTexCoord4sv) GetProcAddress(opengl_module, "glTexCoord4sv");
	if (opengl_context.glTexCoord4sv == NULL)
		return false;

	opengl_context.glTexCoordPointer = (OPENGL_FUNC_glTexCoordPointer) GetProcAddress(opengl_module, "glTexCoordPointer");
	if (opengl_context.glTexCoordPointer == NULL)
		return false;

	opengl_context.glTexEnvf = (OPENGL_FUNC_glTexEnvf) GetProcAddress(opengl_module, "glTexEnvf");
	if (opengl_context.glTexEnvf == NULL)
		return false;

	opengl_context.glTexEnvfv = (OPENGL_FUNC_glTexEnvfv) GetProcAddress(opengl_module, "glTexEnvfv");
	if (opengl_context.glTexEnvfv == NULL)
		return false;

	opengl_context.glTexEnvi = (OPENGL_FUNC_glTexEnvi) GetProcAddress(opengl_module, "glTexEnvi");
	if (opengl_context.glTexEnvi == NULL)
		return false;

	opengl_context.glTexEnviv = (OPENGL_FUNC_glTexEnviv) GetProcAddress(opengl_module, "glTexEnviv");
	if (opengl_context.glTexEnviv == NULL)
		return false;

	opengl_context.glTexGend = (OPENGL_FUNC_glTexGend) GetProcAddress(opengl_module, "glTexGend");
	if (opengl_context.glTexGend == NULL)
		return false;

	opengl_context.glTexGendv = (OPENGL_FUNC_glTexGendv) GetProcAddress(opengl_module, "glTexGendv");
	if (opengl_context.glTexGendv == NULL)
		return false;

	opengl_context.glTexGenf = (OPENGL_FUNC_glTexGenf) GetProcAddress(opengl_module, "glTexGenf");
	if (opengl_context.glTexGenf == NULL)
		return false;

	opengl_context.glTexGenfv = (OPENGL_FUNC_glTexGenfv) GetProcAddress(opengl_module, "glTexGenfv");
	if (opengl_context.glTexGenfv == NULL)
		return false;

	opengl_context.glTexGeni = (OPENGL_FUNC_glTexGeni) GetProcAddress(opengl_module, "glTexGeni");
	if (opengl_context.glTexGeni == NULL)
		return false;

	opengl_context.glTexGeniv = (OPENGL_FUNC_glTexGeniv) GetProcAddress(opengl_module, "glTexGeniv");
	if (opengl_context.glTexGeniv == NULL)
		return false;

	opengl_context.glTexImage1D = (OPENGL_FUNC_glTexImage1D) GetProcAddress(opengl_module, "glTexImage1D");
	if (opengl_context.glTexImage1D == NULL)
		return false;

	opengl_context.glTexImage2D = (OPENGL_FUNC_glTexImage2D) GetProcAddress(opengl_module, "glTexImage2D");
	if (opengl_context.glTexImage2D == NULL)
		return false;

	opengl_context.glTexParameterf = (OPENGL_FUNC_glTexParameterf) GetProcAddress(opengl_module, "glTexParameterf");
	if (opengl_context.glTexParameterf == NULL)
		return false;

	opengl_context.glTexParameterfv = (OPENGL_FUNC_glTexParameterfv) GetProcAddress(opengl_module, "glTexParameterfv");
	if (opengl_context.glTexParameterfv == NULL)
		return false;

	opengl_context.glTexParameteri = (OPENGL_FUNC_glTexParameteri) GetProcAddress(opengl_module, "glTexParameteri");
	if (opengl_context.glTexParameteri == NULL)
		return false;

	opengl_context.glTexParameteriv = (OPENGL_FUNC_glTexParameteriv) GetProcAddress(opengl_module, "glTexParameteriv");
	if (opengl_context.glTexParameteriv == NULL)
		return false;

	opengl_context.glTexSubImage1D = (OPENGL_FUNC_glTexSubImage1D) GetProcAddress(opengl_module, "glTexSubImage1D");
	if (opengl_context.glTexSubImage1D == NULL)
		return false;

	opengl_context.glTexSubImage2D = (OPENGL_FUNC_glTexSubImage2D) GetProcAddress(opengl_module, "glTexSubImage2D");
	if (opengl_context.glTexSubImage2D == NULL)
		return false;

	opengl_context.glTranslated = (OPENGL_FUNC_glTranslated) GetProcAddress(opengl_module, "glTranslated");
	if (opengl_context.glTranslated == NULL)
		return false;

	opengl_context.glTranslatef = (OPENGL_FUNC_glTranslatef) GetProcAddress(opengl_module, "glTranslatef");
	if (opengl_context.glTranslatef == NULL)
		return false;

	opengl_context.glVertex2d = (OPENGL_FUNC_glVertex2d) GetProcAddress(opengl_module, "glVertex2d");
	if (opengl_context.glVertex2d == NULL)
		return false;

	opengl_context.glVertex2dv = (OPENGL_FUNC_glVertex2dv) GetProcAddress(opengl_module, "glVertex2dv");
	if (opengl_context.glVertex2dv == NULL)
		return false;

	opengl_context.glVertex2f = (OPENGL_FUNC_glVertex2f) GetProcAddress(opengl_module, "glVertex2f");
	if (opengl_context.glVertex2f == NULL)
		return false;

	opengl_context.glVertex2fv = (OPENGL_FUNC_glVertex2fv) GetProcAddress(opengl_module, "glVertex2fv");
	if (opengl_context.glVertex2fv == NULL)
		return false;

	opengl_context.glVertex2i = (OPENGL_FUNC_glVertex2i) GetProcAddress(opengl_module, "glVertex2i");
	if (opengl_context.glVertex2i == NULL)
		return false;

	opengl_context.glVertex2iv = (OPENGL_FUNC_glVertex2iv) GetProcAddress(opengl_module, "glVertex2iv");
	if (opengl_context.glVertex2iv == NULL)
		return false;

	opengl_context.glVertex2s = (OPENGL_FUNC_glVertex2s) GetProcAddress(opengl_module, "glVertex2s");
	if (opengl_context.glVertex2s == NULL)
		return false;

	opengl_context.glVertex2sv = (OPENGL_FUNC_glVertex2sv) GetProcAddress(opengl_module, "glVertex2sv");
	if (opengl_context.glVertex2sv == NULL)
		return false;

	opengl_context.glVertex3d = (OPENGL_FUNC_glVertex3d) GetProcAddress(opengl_module, "glVertex3d");
	if (opengl_context.glVertex3d == NULL)
		return false;

	opengl_context.glVertex3dv = (OPENGL_FUNC_glVertex3dv) GetProcAddress(opengl_module, "glVertex3dv");
	if (opengl_context.glVertex3dv == NULL)
		return false;

	opengl_context.glVertex3f = (OPENGL_FUNC_glVertex3f) GetProcAddress(opengl_module, "glVertex3f");
	if (opengl_context.glVertex3f == NULL)
		return false;

	opengl_context.glVertex3fv = (OPENGL_FUNC_glVertex3fv) GetProcAddress(opengl_module, "glVertex3fv");
	if (opengl_context.glVertex3fv == NULL)
		return false;

	opengl_context.glVertex3i = (OPENGL_FUNC_glVertex3i) GetProcAddress(opengl_module, "glVertex3i");
	if (opengl_context.glVertex3i == NULL)
		return false;

	opengl_context.glVertex3iv = (OPENGL_FUNC_glVertex3iv) GetProcAddress(opengl_module, "glVertex3iv");
	if (opengl_context.glVertex3iv == NULL)
		return false;

	opengl_context.glVertex3s = (OPENGL_FUNC_glVertex3s) GetProcAddress(opengl_module, "glVertex3s");
	if (opengl_context.glVertex3s == NULL)
		return false;

	opengl_context.glVertex3sv = (OPENGL_FUNC_glVertex3sv) GetProcAddress(opengl_module, "glVertex3sv");
	if (opengl_context.glVertex3sv == NULL)
		return false;

	opengl_context.glVertex4d = (OPENGL_FUNC_glVertex4d) GetProcAddress(opengl_module, "glVertex4d");
	if (opengl_context.glVertex4d == NULL)
		return false;

	opengl_context.glVertex4dv = (OPENGL_FUNC_glVertex4dv) GetProcAddress(opengl_module, "glVertex4dv");
	if (opengl_context.glVertex4dv == NULL)
		return false;

	opengl_context.glVertex4f = (OPENGL_FUNC_glVertex4f) GetProcAddress(opengl_module, "glVertex4f");
	if (opengl_context.glVertex4f == NULL)
		return false;

	opengl_context.glVertex4fv = (OPENGL_FUNC_glVertex4fv) GetProcAddress(opengl_module, "glVertex4fv");
	if (opengl_context.glVertex4fv == NULL)
		return false;

	opengl_context.glVertex4i = (OPENGL_FUNC_glVertex4i) GetProcAddress(opengl_module, "glVertex4i");
	if (opengl_context.glVertex4i == NULL)
		return false;

	opengl_context.glVertex4iv = (OPENGL_FUNC_glVertex4iv) GetProcAddress(opengl_module, "glVertex4iv");
	if (opengl_context.glVertex4iv == NULL)
		return false;

	opengl_context.glVertex4s = (OPENGL_FUNC_glVertex4s) GetProcAddress(opengl_module, "glVertex4s");
	if (opengl_context.glVertex4s == NULL)
		return false;

	opengl_context.glVertex4sv = (OPENGL_FUNC_glVertex4sv) GetProcAddress(opengl_module, "glVertex4sv");
	if (opengl_context.glVertex4sv == NULL)
		return false;

	opengl_context.glVertexPointer = (OPENGL_FUNC_glVertexPointer) GetProcAddress(opengl_module, "glVertexPointer");
	if (opengl_context.glVertexPointer == NULL)
		return false;

	opengl_context.glViewport = (OPENGL_FUNC_glViewport) GetProcAddress(opengl_module, "glViewport");
	if (opengl_context.glViewport == NULL)
		return false;

	return true;
}
