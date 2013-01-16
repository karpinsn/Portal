#include "OpenGLTripleBuffer.h"

OpenGLTripleBuffer::OpenGLTripleBuffer(ISharedGLContextFactory& contextFactory) :
  m_contextFactory(contextFactory)
{ }

void OpenGLTripleBuffer::initWrite(int width, int height)
{
  //  Get a context for use to use with writes
  m_writeContext = m_contextFactory.MakeSharedContext();

  //  TODO: Initilize our buffers to the correct width and height
}

void OpenGLTripleBuffer::write(void* data, int size)
{
  m_writeContext->makeCurrent();

  //  TODO: Write the actual data
}