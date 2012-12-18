#include "GLUTManager.h"

GLUTManager::GLUTManager(IGLContext* context, int argc, char* argv[]) : m_context(context)
{
  //  Initialize GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(800, 600);
  glutCreateWindow("GLUT Window");

  //  Register ourselves as the callback context
  g_glutContext = this;

  //	Setup the glut callback functions
  glutDisplayFunc(glutDisplay); 
  glutReshapeFunc(glutReshape);
  glutMouseFunc  (glutMouseFunction);
  glutMotionFunc (glutMotionFunction);
}

void GLUTManager::start(void)
{
    // Starts the main rendering loop and never returns
    glutMainLoop();
}

// ------------------------ Globals used by GLUT ------------------------------
GLUTManager* g_glutContext;

void glutDisplay(void)
{
  if(nullptr != g_glutContext)
  {
	g_glutContext->m_context->draw(); 
	glutSwapBuffers();
  }
}

void glutReshape(int width, int height)
{
  if(nullptr != g_glutContext)
	g_glutContext->m_context->resize(width, height);
}

void glutMouseFunction(int button, int state, int mouseX, int mouseY)
{
  //  TODO: Also make sure that we are pressing the right mouse button
  if(nullptr != g_glutContext)
	g_glutContext->m_context->mousePressEvent(mouseX, mouseY);
}

void glutMotionFunction(int mouseX, int mouseY)
{
  if(nullptr != g_glutContext)
	g_glutContext->m_context->mouseMoveEvent(mouseX, mouseY);
}