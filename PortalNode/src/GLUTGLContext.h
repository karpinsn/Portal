/**
@file
@author Nikolaus Karpinsky
@since  12/15/2012 
*/

#ifndef _GLUT_GL_CONTEXT_H_
#define _GLUT_GL_CONTEXT_H_

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

#include <gl/glut.h>

#include "IGLWidget.h"

using namespace std;

class GLUTGLContext : public IGLWidget
{
public:
	void init(void);
    void init(int width, int height);
    void start(void);
	void draw(void);
	void resize(int width, int height);
	void cameraSelectMode(int mode);
	void mousePressEvent(int mouseX, int mouseY);
	void mouseMoveEvent(int mouseX, int mouseY);
};

// ------------------- Globals used by GLUT -----------------------------------
GLUTGLContext* g_glutContext;
void glutDisplay(void);
// ----------------------------------------------------------------------------

#endif	// _GLUT_GL_CONTEXT_H_
