/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_OPEN_GL_TRIPLE_BUFFER_H_
#define _PORTAL_CAPTURE_OPEN_GL_TRIPLE_BUFFER_H_

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

#include <QGLContext>
#include <QMutex>

#include <wrench\gl\Texture.h>

#include "ISharedGLContextFactory.h"
#include "Utils.h"
#include "IReadBuffer.h"
#include "IWriteBuffer.h"

using namespace wrench::gl;

class OpenGLTripleBuffer : public IReadBuffer, public IWriteBuffer
{
private:
  DISALLOW_COPY_AND_ASSIGN(OpenGLTripleBuffer);
  ISharedGLContextFactory& m_contextFactory;
  
  shared_ptr<QGLContext> m_writeContext;

  unique_ptr<Texture> m_writeBuffer;
  unique_ptr<Texture> m_workingBuffer;
  unique_ptr<Texture> m_readBuffer;

  QMutex m_swapLock;

public:
  OpenGLTripleBuffer(ISharedGLContextFactory& contextFactory);
  virtual void initWrite(int width, int height);
  virtual void write(const IplImage* data);
};

#endif // _PORTAL_CAPTURE_OPEN_GL_TRIPLE_BUFFER_H_
