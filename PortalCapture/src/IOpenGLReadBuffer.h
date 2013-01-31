/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_I_OPEN_GL_READ_BUFFER_H_
#define _PORTAL_CAPTURE_I_OPEN_GL_READ_BUFFER_H_

#ifdef __APPLE__
#include <glew.h>
#include <OpenGL/gl.h>
#elif _WIN32
#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#endif

#include <QObject>

#include <cv.h>
#include <highgui.h>

class IReadBuffer
{
public:
  virtual int						  GetWidth( void )	  = 0;
  virtual int						  GetHeight( void )	  = 0;
  virtual const shared_ptr<IplImage>  ReadBuffer( void )  = 0;
};

class IOpenGLReadBuffer : public QObject, public IReadBuffer
{
  Q_OBJECT

public:
  virtual void	BindBuffer( GLenum texture ) = 0;
};

#endif // _PORTAL_CAPTURE_I_OPEN_GL_READ_BUFFER_H_
