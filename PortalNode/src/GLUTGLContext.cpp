#include "GLUTGLContext.h"

void GLUTGLContext::init(void)
{
    //glutInit(&argc, argv);
    
    //glut initialization
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(800, 600);
    g_iMainWindow = glutCreateWindow("GLUT Window");
    glutDisplayFunc(glutDisplay);
}

void GLUTGLContext::init(int width, int height)
{
}

void GLUTGLContext::start(void)
{
    // Starts the main rendering loop and never returns
    glutMainLoop();
}

void GLUTGLContext::draw(void)
{
}

void GLUTGLContext::resize(int width, int height)
{
    glutPostRedisplay();
}

void GLUTGLContext::cameraSelectMode(int mode)
{
    glutPostRedisplay();
}

void GLUTGLContext::mousePressEvent(int mouseX, int mouseY)
{
    glutPostRedisplay();
}

void GLUTGLContext::mouseMoveEvent(int mouseX, int mouseY)
{
    glutPostRedisplay();
}

// ------------------------ Globals used by GLUT ------------------------------
void glutDisplay(void)
{
    if(nullptr != g_glutContext)
       g_glutContext->draw(); 
}

