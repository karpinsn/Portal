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

#include <QObject>
#include <QScriptEngine>
#include <QtScript>
#include <QMetaType>

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
  //DISALLOW_COPY_AND_ASSIGN(MultiOpenGLBuffer);
  ::vector<shared_ptr<OpenGLTripleBuffer>> m_buffers;
  ::size_t m_currentBufferIndex;

public:
  Q_INVOKABLE MultiOpenGLBuffer(::size_t bufferCount, bool makeReadContext, bool makeWriteContext, ISharedGLContextFactory* contextFactory);
  
  // Write buffers
  void		InitWrite(int width, int height);
  void		Write(const IplImage* data);
  Texture&	StartWriteTexture( void );
  void		WriteFinished( void );

  // Read Buffers
  void						  StartRead( void );
  const shared_ptr<IplImage>  ReadImage( void );
  const wrench::gl::Texture&  ReadTexture( void );

  // Read buffers
  int						  GetWidth( void );
  int						  GetHeight( void );
  ::vector<shared_ptr<OpenGLTripleBuffer>>::const_iterator ReadBuffersBegin( void );
  ::vector<shared_ptr<OpenGLTripleBuffer>>::const_iterator ReadBuffersEnd( void );

signals:
  //  Emitted when the write buffers have been filled up
  void WriteFilled( void );
};

Q_DECLARE_METATYPE(MultiOpenGLBuffer*)

#endif // _PORTAL_CAPTURE_MULTI_OPEN_GL_BUFFER_H_
