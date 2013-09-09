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

#include <memory>

#include <QObject>
#include <QGLWidget>
#include <QMutex>
#include <QMetaType>

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
  const unsigned int m_channelCount;

  unique_ptr<Texture> m_writeBuffer;
  unique_ptr<Texture> m_workingBuffer;
  unique_ptr<Texture> m_readBuffer;

  shared_ptr<IplImage> m_readImage;

  QMutex m_swapLock;

public:
  OpenGLTripleBuffer(ISharedGLContextFactory* contextFactory, bool makeReadContext, bool makeWriteContext, unsigned int channelCount = 3 );
  
  void						  InitWrite(int width, int height);
  void						  Write(const IplImage* data);
  Texture&					  StartWriteTexture( void );
  void						  WriteFinished( void );
  void						  SwapWriteBuffer( void );

  int						  GetWidth( void );
  int						  GetHeight( void );
  void						  StartRead( void );
  const shared_ptr<IplImage>  ReadImage( void );
  const Texture&			  ReadTexture( void );

private:
  void _swapWriteBuffer( void );
  void _swapReadBuffer( void );

signals:
  //  Emitted when the write buffers have been filled up
  void WriteFilled( void );
};

Q_DECLARE_METATYPE(OpenGLTripleBuffer*)

#endif // _PORTAL_CAPTURE_OPEN_GL_TRIPLE_BUFFER_H_
