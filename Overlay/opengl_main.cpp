#include "opengl_main.h"

#include <math.h>

// taken from http://nehe.gamedev.net
// thanks, NeHe!
void PerspectiveGL_P(OPENGL_CONTEXT opengl_context, GLdouble fov_y, GLdouble aspect, GLdouble z_near, GLdouble z_far)
{
    const GLdouble pi = 3.1415926535897932384626433832795;
    GLdouble w, h;

    h = tan((fov_y / 2) / 180 * pi) * z_near;
    h = tan(fov_y / 360 * pi) * z_near;
    w = h * aspect;

    opengl_context.glFrustum(-w, w, -h, h, z_near, z_far);
}

bool LoadContent(OPENGL_CONTEXT & opengl_context)
{
	return true;
}

void UnloadContent(OPENGL_CONTEXT & opengl_context)
{
}

bool Draw(OPENGL_CONTEXT & opengl_context)
{
	// save the states that are going to be modified
	int depth_func;
	GLboolean depth_test = opengl_context.glIsEnabled(GL_DEPTH_TEST);
	opengl_context.glGetIntegerv(GL_DEPTH_FUNC, &depth_func);

	GLboolean lighting = opengl_context.glIsEnabled(GL_LIGHTING);
	GLboolean blend_enabled = opengl_context.glIsEnabled(GL_BLEND);
	GLboolean texture1d_enabled = opengl_context.glIsEnabled(GL_TEXTURE_1D);
	GLboolean texture2d_enabled = opengl_context.glIsEnabled(GL_TEXTURE_2D);

	opengl_context.glEnable(GL_DEPTH_TEST);
	opengl_context.glDepthFunc(GL_ALWAYS);

	opengl_context.glDisable(GL_LIGHTING);
	opengl_context.glDisable(GL_BLEND);
	opengl_context.glDisable(GL_TEXTURE_1D | GL_TEXTURE_2D);

	int matrix_mode;
	opengl_context.glGetIntegerv(GL_MATRIX_MODE, &matrix_mode);

	opengl_context.glMatrixMode(GL_PROJECTION);
	opengl_context.glPushMatrix();
	opengl_context.glLoadIdentity();

	PerspectiveGL_P(opengl_context, 90.0f, 640.0f / 480.0f, 0.1f, 100.0f);

	opengl_context.glMatrixMode(GL_MODELVIEW);
	opengl_context.glPushMatrix();

	// draw the test geometry
	static float rotation = 0;
	static DWORD last_call_time = GetTickCount() - 1;

	opengl_context.glLoadIdentity();
	opengl_context.glTranslatef(0.0f, 0.0f, -3.0f);

	rotation += (GetTickCount() - last_call_time) * 15.0f / 1000.f;
	opengl_context.glRotatef(rotation, 1.0f, 0.0f, 0.0f);
	opengl_context.glRotatef(rotation, 0.0f, 1.0f, 0.0f);
	opengl_context.glRotatef(rotation, 0.0f, 0.0f, 1.0f);

	opengl_context.glBegin(GL_QUADS);

	opengl_context.glColor3f(0.0f, 1.0f, 0.0f);
	opengl_context.glVertex3f(-1.0f, 1.0f, 0.0f);
	opengl_context.glVertex3f(1.0f, 1.0f, 0.0f);
	opengl_context.glVertex3f(1.0f, -1.0f, 0.0f);
	opengl_context.glVertex3f(-1.0f, -1.0f, 0.0f);

    opengl_context.glEnd();

	last_call_time = GetTickCount();

	// restore the states that have been modified
	opengl_context.glMatrixMode(GL_PROJECTION);
	opengl_context.glPopMatrix();

	opengl_context.glMatrixMode(GL_MODELVIEW);
	opengl_context.glPopMatrix();

	opengl_context.glMatrixMode(matrix_mode);

	if (depth_test == GL_TRUE)
		opengl_context.glEnable(GL_DEPTH_TEST);
	else
		opengl_context.glDisable(GL_DEPTH_TEST);
	opengl_context.glDepthFunc(depth_func);

	if (lighting == GL_TRUE)
		opengl_context.glEnable(GL_LIGHTING);
	else
		opengl_context.glDisable(GL_LIGHTING);

	if (blend_enabled == GL_TRUE)
		opengl_context.glEnable(GL_BLEND);
	else
		opengl_context.glDisable(GL_BLEND);

	if (texture1d_enabled == GL_TRUE)
		opengl_context.glEnable(GL_TEXTURE_1D);
	else
		opengl_context.glDisable(GL_TEXTURE_2D);

	if (texture2d_enabled == GL_TRUE)
		opengl_context.glEnable(GL_TEXTURE_2D);
	else
		opengl_context.glDisable(GL_TEXTURE_2D);

	return true;
}
