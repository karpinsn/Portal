#include "MultiOpenGLBuffer.h"

MultiOpenGLBuffer::MultiOpenGLBuffer(::size_t bufferCount, bool makeReadContext, bool makeWriteContext, ISharedGLContextFactory* contextFactory) :
  m_currentBufferIndex(0)
{ 
  // Make all of the buffers that we need
  for ( ::size_t buffer = 0; buffer < bufferCount; ++buffer )
  { 
	m_buffers.push_back(make_shared<OpenGLTripleBuffer>( contextFactory, makeReadContext, makeWriteContext ) );
  }

  // Try and use as little memory as possible
  m_buffers.shrink_to_fit();
}

void MultiOpenGLBuffer::InitWrite(int width, int height)
{
  // Init all of our buffers
  for ( auto iterator = m_buffers.begin( ); iterator != m_buffers.end( ); ++iterator )
  {
	(*iterator)->InitWrite( width, height );
  }
}

void MultiOpenGLBuffer::Write(const IplImage* data)
{
  Utils::AssertOrThrowIfFalse(m_currentBufferIndex < m_buffers.size( ), "Trying to access a buffer that we dont have");

  //  Write to the current buffer
  m_buffers[m_currentBufferIndex]->Write(data);

  m_currentBufferIndex++;
  if ( m_currentBufferIndex >= m_buffers.size() )
  {
	m_currentBufferIndex = 0;
	emit( WriteFilled( ) );
  }
}

Texture& MultiOpenGLBuffer::WriteBuffer( void )
{
  throw "UNIMPLEMENTED!!!!";
}

void MultiOpenGLBuffer::WriteFinished( void )
{
  throw "UNIMPLEMENTED!!!!";
}

const shared_ptr<IplImage> MultiOpenGLBuffer::ReadBuffer( void )
{
  throw "UNIMPLEMENTED!!!!";
  return nullptr;
}

int MultiOpenGLBuffer::GetWidth( void )
{
  return m_buffers[0]->GetWidth();
}

int MultiOpenGLBuffer::GetHeight( void )
{
  return m_buffers[0]->GetHeight();
}

void MultiOpenGLBuffer::BindBuffer(GLenum texture)
{
  for ( ::size_t buffer = 0; buffer < m_buffers.size(); ++buffer )
  {
	m_buffers[buffer]->BindBuffer(GL_TEXTURE0 + buffer);
  }
}