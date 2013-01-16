#include "MainController.h"

MainController::MainController() : QObject()
{

}

void MainController::Init(void)
{
  // By creating a pixel buffer we will get a valid QGLContext without needing
  // a UI. We must have an actual screen though (X Session or Windows session)
  // so that we can get a graphics context, but we wont actually show a window
  m_mainBuffer = unique_ptr<QGLPixelBuffer>( new QGLPixelBuffer( 8, 8 ) ); // 8 is used because it is a small power of 2

  //  Make sure we support pBuffers and have a valid context
  Utils::AssertOrThrowIfFalse( m_mainBuffer->hasOpenGLPbuffers( ), "Missing Pixel Buffer support. Can't create an OpenGL Context" );
  Utils::AssertOrThrowIfFalse( m_mainBuffer->isValid( ), "OpenGL Context is not valid" );
  Utils::AssertOrThrowIfFalse( m_mainBuffer->makeCurrent( ), "Unable to attach to the OpenGL context" );
  
  // Now create the main context (shared with the QGLPixelBuffer). This will be
  // used by other contexts to share texture access etc...
  m_mainContext = unique_ptr<QGLContext>( new QGLContext( QGLFormat( ), m_mainBuffer.get( ) ) );
  m_mainContext->makeCurrent( );

  //  Now I need to get access to the context to share it with additional threads
  m_captureContext = unique_ptr<ICaptureContext>( new CameraCapture( ) );
  m_captureContext->Init( );
}

void MainController::Start(void)
{
  m_captureContext->Start( );
}

void MainController::Close(void)
{
  //  This will tell the event loop that we are done and close the app
  emit( Finished( ) );
}