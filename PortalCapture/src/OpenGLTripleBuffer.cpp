#include "OpenGLTripleBuffer.h"

OpenGLTripleBuffer::OpenGLTripleBuffer(ISharedGLContextFactory* contextFactory, bool makeReadContext, bool makeWriteContext) :
  m_contextFactory(contextFactory), m_writeContext(nullptr), m_readContext(nullptr), m_makeReadContext(makeReadContext), m_makeWriteContext(makeWriteContext)
{ }

void OpenGLTripleBuffer::InitWrite(int width, int height)
{
  Utils::AssertOrThrowIfFalse(width > 0, "Must have a valid width > 0 for the image buffer");
  Utils::AssertOrThrowIfFalse(height > 0, "Must have a valid height > 0 for the image buffer");

  // If needed get a context for use to use with writes
  if( nullptr != m_contextFactory )
  {
	//	TODO
	//if ( m_makeReadContext )
	//{
	//  m_readContext = m_contextFactory->MakeSharedContext();
	//  //  TODO: Should this be here?
	//  //m_readContext->makeCurrent( );
	//}

	if ( m_makeWriteContext )
	{
	  m_writeContext = m_contextFactory->MakeSharedContext();
	  //  TODO: Should this be here?
	  m_writeContext->makeCurrent( );
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
  m_swapLock.lock();
  {
	m_writeBuffer.swap(m_workingBuffer);
  }
  m_swapLock.unlock();
  emit ( WriteFilled( ) );
}

const Texture& OpenGLTripleBuffer::WriteBuffer( void )
{
  if( nullptr != m_writeContext )
	{ m_writeContext->makeCurrent(); }

  return *m_writeBuffer.get();
}

void OpenGLTripleBuffer::WriteFinished( void )
{
  if( nullptr != m_writeContext )
	{ m_writeContext->makeCurrent(); }
  m_swapLock.lock();
  {
	m_writeBuffer.swap(m_workingBuffer);
  }
  m_swapLock.unlock();
  emit ( WriteFilled( ) );
}

void OpenGLTripleBuffer::InitRead( void )
{
  if ( m_makeReadContext && nullptr == m_readContext)
  {
	m_readContext = m_contextFactory->MakeSharedContext();
  }
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
  if( nullptr != m_readContext )
	{ m_readContext->makeCurrent(); }

  m_swapLock.lock();
  {
	m_readBuffer.swap(m_workingBuffer);
  }
  m_swapLock.unlock();

  m_readBuffer->transferFromTexture( m_readImage.get( ) );
  return m_readImage;
}

void OpenGLTripleBuffer::BindBuffer(GLenum texture)
{
  if( nullptr != m_readContext )
	{ m_readContext->makeCurrent(); }

  m_swapLock.lock();
  {
	m_readBuffer.swap(m_workingBuffer);
  }
  m_swapLock.unlock();

  m_readBuffer->bind(texture);
}