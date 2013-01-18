/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_I_OPEN_GL_READ_BUFFER_H_
#define _PORTAL_CAPTURE_I_OPEN_GL_READ_BUFFER_H_

class IOpenGLReadBuffer
{
public:
  virtual void	BindBuffer( GLenum texture ) = 0;
  virtual int	GetWidth( void ) = 0;
  virtual int	GetHeight( void ) = 0;
};

#endif // _PORTAL_CAPTURE_I_OPEN_GL_READ_BUFFER_H_
