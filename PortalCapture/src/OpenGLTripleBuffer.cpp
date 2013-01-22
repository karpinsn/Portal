#include "OpenGLTripleBuffer.h"

OpenGLTripleBuffer::OpenGLTripleBuffer(ISharedGLContextFactory* contextFactory) :
  m_contextFactory(contextFactory), m_writeContext(nullptr)
{ }

void OpenGLTripleBuffer::InitWrite(int width, int height)
{
  // If needed get a context for use to use with writes
  if(nullptr != m_contextFactory)
  {
	m_writeContext = m_contextFactory->MakeSharedContext();
	m_writeContext->makeCurrent( );
  }

  m_writeBuffer	  = unique_ptr<Texture>( new Texture( ) );
  m_workingBuffer = unique_ptr<Texture>( new Texture( ) );
  m_readBuffer	  = unique_ptr<Texture>( new Texture( ) );

  m_writeBuffer->init	(width, height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
  m_workingBuffer->init	(width, height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
  m_readBuffer->init	(width, height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);

  m_readImage = shared_ptr<IplImage>( cvCreateImage( cvSize( width, height), IPL_DEPTH_8U, 3 ),
	[] ( IplImage* ptr ) { cvReleaseImage( &ptr ); } );
}

void OpenGLTripleBuffer::Write(const IplImage* data)
{
  if( nullptr != m_writeContext )
	{ m_writeContext->makeCurrent(); }
  
  m_writeBuffer->transferToTexture(data);
  m_swapLock.lock();
  {
	m_writeBuffer.swap(m_workingBuffer);
  }
  m_swapLock.unlock();
  emit ( WriteFilled( ) );
}

const Texture& OpenGLTripleBuffer::WriteBuffer( void )
{
  return *m_writeBuffer.get();
}

int OpenGLTripleBuffer::GetWidth( void )
{
  return m_readBuffer->getWidth();
}

int OpenGLTripleBuffer::GetHeight( void )
{
  return m_readBuffer->getHeight();
}

const shared_ptr<IplImage> OpenGLTripleBuffer::ReadBuffer( void )
{
  m_readBuffer->transferFromTexture( m_readImage.get( ) );
  return m_readImage;
}

void OpenGLTripleBuffer::BindBuffer(GLenum texture)
{
  m_swapLock.lock();
  {
	m_readBuffer.swap(m_workingBuffer);
  }
  m_swapLock.unlock();

  m_readBuffer->bind(texture);
}