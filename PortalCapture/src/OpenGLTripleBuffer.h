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

#include <QObject>
#include <QGLWidget>
#include <QMutex>

#include <wrench\gl\Texture.h>

#include "ISharedGLContextFactory.h"
#include "Utils.h"
#include "ITripleBuffer.h"

using namespace wrench::gl;

class OpenGLTripleBuffer : public ITripleBuffer
{
  Q_OBJECT

private:
  DISALLOW_COPY_AND_ASSIGN(OpenGLTripleBuffer);
  ISharedGLContextFactory* m_contextFactory;
  
  shared_ptr<QGLWidget> m_writeContext;
  shared_ptr<QGLWidget> m_readContext;
  bool m_makeReadContext;
  bool m_makeWriteContext;

  unique_ptr<Texture> m_writeBuffer;
  unique_ptr<Texture> m_workingBuffer;
  unique_ptr<Texture> m_readBuffer;

  shared_ptr<IplImage> m_readImage;

  QMutex m_swapLock;

public:
  OpenGLTripleBuffer(ISharedGLContextFactory* contextFactory, bool makeReadContext, bool makeWriteContext);
  
  void						  InitWrite(int width, int height);
  void						  Write(const IplImage* data);
  Texture&					  WriteBuffer( void );
  void						  WriteFinished( void );

  int						  GetWidth( void );
  int						  GetHeight( void );
  const shared_ptr<IplImage>  ReadBuffer( void );
  void						  BindBuffer( GLenum texture );

signals:
  //  Emitted when the write buffers have been filled up
  void WriteFilled( void );
};

#endif // _PORTAL_CAPTURE_OPEN_GL_TRIPLE_BUFFER_H_
