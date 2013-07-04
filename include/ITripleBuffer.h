/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_I_TRIPLE_BUFFER_H_
#define _PORTAL_CAPTURE_I_TRIPLE_BUFFER_H_

#include <memory>

#include <QMetaType>

#include <wrench/gl/Texture.h>

#include "Utils.h"

class ITripleBuffer : public QObject
{
  Q_OBJECT

	// TODO: Look into putting this back in
//private:
  //DISALLOW_COPY_AND_ASSIGN(ITripleBuffer);

public:	// Read Buffer
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

public: // Write Buffer
  virtual void		            InitWrite(int width, int height)  = 0;
  virtual void                  Write(const IplImage* data)		  = 0;
  virtual wrench::gl::Texture&	StartWriteTexture( void )		  = 0;
  virtual void		            WriteFinished( void )			  = 0;
};

Q_DECLARE_METATYPE(ITripleBuffer*)

#endif // _PORTAL_CAPTURE_I_TRIPLE_BUFFER_H_
