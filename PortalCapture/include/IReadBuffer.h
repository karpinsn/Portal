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

#include <memory>

#include <QObject>

#include <cv.h>
#include <highgui.h>

#include <wrench\gl\Texture.h>

using namespace std;

class IReadBuffer : public QObject
{
  Q_OBJECT

public:
  virtual int						  GetWidth( void )		= 0;
  virtual int						  GetHeight( void )		= 0;

  /**
	* Starts a buffer read. This effictively swaps the read and working
	* buffers. If this is not called, any Read* call will read from the
	* previous read buffer
	*/
  virtual void						  StartRead( void )		= 0;

  /**
	* Returns the current read buffer as an IplImage.
	*
	* Returns the current read buffer as an IplImage. If a buffer swap
	* needs to be performed (Typically done before a read) then
	* a call to StartRead must be performed.
	*
	* @return Shared Pointer of an IplImage to the current read buffer.
	*/
  virtual const shared_ptr<IplImage>  ReadImage( void )	= 0;

  /**
	* Returns the current read buffer as a wrench::Texture
	*
	* Returns the current read buffer as a wrench::Texture. If a buffer swap
	* needs to be performed (Typically done before a read) then a
	* call to StartRead must be performed.
	*
	* @return const reference to the wrench::Texture object
	*/
  virtual const wrench::gl::Texture&  ReadTexture( void ) = 0;
};

#endif // _PORTAL_CAPTURE_I_OPEN_GL_READ_BUFFER_H_
