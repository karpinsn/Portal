#include "OpenGLTripleBuffer.h"

OpenGLTripleBuffer::OpenGLTripleBuffer(ISharedGLContextFactory& contextFactory) :
  m_contextFactory(contextFactory)
{ }

void OpenGLTripleBuffer::initWrite(int width, int height)
{
  //  Get a context for use to use with writes
  m_writeContext = m_contextFactory.MakeSharedContext();
  m_writeContext->makeCurrent( );

  m_writeBuffer	  = unique_ptr<Texture>( new Texture( ) );
  m_workingBuffer = unique_ptr<Texture>( new Texture( ) );
  m_readBuffer	  = unique_ptr<Texture>( new Texture( ) );

  m_writeBuffer->init	(width, height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
  m_workingBuffer->init	(width, height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
  m_readBuffer->init	(width, height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
}

void OpenGLTripleBuffer::write(const IplImage* data)
{
  m_writeContext->makeCurrent();
  m_writeBuffer->transferToTexture(data);

  m_swapLock.lock();
  {
	m_writeBuffer.swap(m_workingBuffer);
  }
  m_swapLock.unlock();
}