/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_OPEN_GL_TRIPLE_BUFFER_H_
#define _PORTAL_CAPTURE_OPEN_GL_TRIPLE_BUFFER_H_

#include "Utils.h"
#include "IReadBuffer.h"
#include "IWriteBuffer.h"

class OpenGLTripleBuffer : public IReadBuffer, public IWriteBuffer
{
private:
  DISALLOW_COPY_AND_ASSIGN(OpenGLTripleBuffer);

public:
  virtual void initWrite(int width, int height);
  virtual void write(void* data, int size);
};

#endif // _PORTAL_CAPTURE_OPEN_GL_TRIPLE_BUFFER_H_
