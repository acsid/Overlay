#ifndef OPENGL_SUPPORT_H
#define OPENGL_SUPPORT_H

#include <windows.h>
#include <gl\gl.h>

bool WINAPI IsOpenGLLoaded(bool & found);
bool WINAPI OpenGLInitialize();
void WINAPI OpenGLCleanup();

#endif // OPENGL_SUPPORT_H
