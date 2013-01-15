#include "MainController.h"

MainController::MainController() : QObject()
{
}

void MainController::Init(void)
{
  m_mainContext = unique_ptr<QGLContext>( new QGLContext( QGLFormat( QGL::SampleBuffers ) ) );

  m_captureContext = unique_ptr<ICaptureContext>( new CameraCapture() );
  m_captureContext->Init();
}

void MainController::Start(void)
{
  m_captureContext->Start();
}

void MainController::Close(void)
{
  

  //  This will tell the event loop that we are done and close the app
  emit( Finished() );
}