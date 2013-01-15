/**
@file
@author Nikolaus Karpinsky
@since  09/01/09
*/

#ifndef _PORTAL_GL_CONTEXT_H_
#define _PORTAL_GL_CONTEXT_H_

#ifdef __APPLE__
#include <glew.h>
#elif _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <GL/glew.h>
#else
#include <GL/glew.h>
#endif

#include "IGLContext.h"

using namespace std;

class PortalGLContext : public IGLContext
{
public:
	void init(void);
    void init(int width, int height);
	void draw(void);
	void resize(int width, int height);
	void cameraSelectMode(int mode);
	void mousePressEvent(int mouseX, int mouseY);
	void mouseMoveEvent(int mouseX, int mouseY);
};

#endif	// _PORTAL_GL_CONTEXT_H_
