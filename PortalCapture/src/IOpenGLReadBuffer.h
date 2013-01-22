/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_I_OPEN_GL_READ_BUFFER_H_
#define _PORTAL_CAPTURE_I_OPEN_GL_READ_BUFFER_H_

class IReadBuffer
{
public:
  
  virtual int						  GetWidth( void )	  = 0;
  virtual int						  GetHeight( void )	  = 0;
  virtual const shared_ptr<IplImage>  ReadBuffer( void )  = 0;
};

class IOpenGLReadBuffer : public IReadBuffer
{
public:
  virtual void	BindBuffer( GLenum texture ) = 0;
};

#endif // _PORTAL_CAPTURE_I_OPEN_GL_READ_BUFFER_H_
