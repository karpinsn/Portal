/**
@file
@author Nikolaus Karpinsky
@since  12/15/2012 
*/

#ifndef _GLUT_MANAGER_H_
#define _GLUT_MANAGER_H_

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

#include <GL/glut.h>

#include "IGLContext.h"

using namespace std;

class GLUTManager
{
public:
    //  This needs to be public so we can access it from C global functions :(
    IGLContext *m_context;

    GLUTManager(IGLContext* widget, int argc, char* argv[]);
    void start(void);
};

// ------------------- Globals used by GLUT -----------------------------------
extern GLUTManager* g_glutContext;
void glutDisplay(void);
void glutReshape(int width, int height);
void glutMouseFunction(int button, int state, int mouseX, int mouseY);
void glutMotionFunction(int mouseX, int mouseY);
// ----------------------------------------------------------------------------

#endif	// _GLUT_MANAGER_H_
