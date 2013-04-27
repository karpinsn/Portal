#include "OpenGLTripleBuffer.h"

OpenGLTripleBuffer::OpenGLTripleBuffer(ISharedGLContextFactory* contextFactory, bool makeReadContext, bool makeWriteContext) :
  m_contextFactory(contextFactory), m_writeContext(nullptr), m_readContext(nullptr), m_makeReadContext(makeReadContext), m_makeWriteContext(makeWriteContext)
{ }

void OpenGLTripleBuffer::InitWrite(int width, int height)
{
  Utils::ThrowIfFalse(width > 0, "Must have a valid width > 0 for the image buffer");
  Utils::ThrowIfFalse(height > 0, "Must have a valid height > 0 for the image buffer");

  // If needed get a context for use to use with writes
  if ( nullptr != m_contextFactory )
  {
	if ( m_makeWriteContext )
	{
	  m_writeContext = m_contextFactory->MakeSharedContext( );
	  m_writeContext->makeCurrent( );
	}
	if ( m_makeReadContext )
	{
	  m_readContext = m_contextFactory->MakeSharedContext( );
	  m_readContext->makeCurrent( );
	}
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
  _swapWriteBuffer();

  emit ( WriteFilled( ) );
}

Texture& OpenGLTripleBuffer::StartWriteTexture( void )
{
  if( nullptr != m_writeContext )
	{ m_writeContext->makeCurrent(); }

  return *m_writeBuffer.get();
}

void OpenGLTripleBuffer::WriteFinished( void )
{
  if( nullptr != m_writeContext )
	{ m_writeContext->makeCurrent(); }
 
  _swapWriteBuffer();
  emit ( WriteFilled( ) );
}

int OpenGLTripleBuffer::GetWidth( void )
{
  return m_readBuffer->getWidth();
}

int OpenGLTripleBuffer::GetHeight( void )
{
  return m_readBuffer->getHeight();
}

void OpenGLTripleBuffer::StartRead( void )
{
  if( nullptr != m_readContext )
	{ m_readContext->makeCurrent(); }

  _swapReadBuffer();
}

const shared_ptr<IplImage> OpenGLTripleBuffer::ReadImage( void )
{
  if( nullptr != m_readContext )
	{ m_readContext->makeCurrent(); }

  m_readBuffer->transferFromTexture( m_readImage.get( ) );
  return m_readImage;
}

const Texture& OpenGLTripleBuffer::ReadTexture( void )
{
  if( nullptr != m_readContext )
	{ m_readContext->makeCurrent(); }

  return *( m_readBuffer.get( ) );
}

void OpenGLTripleBuffer::_swapWriteBuffer( void )
{
  m_swapLock.lock();
  {
	m_writeBuffer.swap(m_workingBuffer);
  }
  m_swapLock.unlock();
}

void OpenGLTripleBuffer::_swapReadBuffer( void )
{
  m_swapLock.lock();
  {
	m_readBuffer.swap(m_workingBuffer);
  }
  m_swapLock.unlock();
}
