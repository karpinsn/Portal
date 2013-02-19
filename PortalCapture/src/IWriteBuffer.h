/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_I_WRITE_BUFFER_H_
#define _PORTAL_CAPTURE_I_WRITE_BUFFER_H_

#include <wrench\gl\Texture.h>

using namespace wrench::gl;

class IWriteBuffer
{
public:
  virtual void InitWrite(int width, int height) = 0;
  virtual void Write(const IplImage* data) = 0;
  virtual Texture& WriteBuffer( void ) = 0;
  virtual void WriteFinished( void ) = 0;
};

#endif // _PORTAL_CAPTURE_I_WRITE_BUFFER_H_
