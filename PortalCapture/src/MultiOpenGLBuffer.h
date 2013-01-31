/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_MULTI_OPEN_GL_BUFFER_H_
#define _PORTAL_CAPTURE_MULTI_OPEN_GL_BUFFER_H_

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

#include <vector>

#include "ITripleBuffer.h"
#include "OpenGLTripleBuffer.h"
#include "ISharedGLContextFactory.h"
#include "Utils.h"

using namespace std;
using namespace wrench::gl;

class MultiOpenGLBuffer : public ITripleBuffer
{
  Q_OBJECT

private:
  DISALLOW_COPY_AND_ASSIGN(MultiOpenGLBuffer);
  ::vector<shared_ptr<OpenGLTripleBuffer>> m_buffers;
  ::size_t m_currentBufferIndex;

public:
  MultiOpenGLBuffer(::size_t bufferCount, ISharedGLContextFactory* contextFactory);
  
  // Write buffers
  void InitWrite(int width, int height);
  void Write(const IplImage* data);
  Texture& WriteBuffer( void );
  void WriteFinished( void );

  // Read buffers
  int						  GetWidth( void );
  int						  GetHeight( void );
  const shared_ptr<IplImage>  ReadBuffer( void );
  void						  BindBuffer( GLenum texture );

signals:
  //  Emitted when the write buffers have been filled up
  void WriteFilled( void );
};

#endif // _PORTAL_CAPTURE_MULTI_OPEN_GL_BUFFER_H_
