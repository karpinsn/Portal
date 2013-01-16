#include "MainController.h"

MainController::MainController() : QObject(), m_mainContext(nullptr), m_captureContext(nullptr)
{ }

void MainController::Init(void)
{
  // By creating a QGLWidget we will get a QGLContext. As long as we
  // dont show it we will have a headless GLContext. Athough headless
  // we must have an actual screen though (X Session or Explorer session)
  // so that we can get a window and graphics context.
  m_mainContext = unique_ptr<QGLWidget>( new QGLWidget( ) );  
  Utils::AssertOrThrowIfFalse(m_mainContext->isValid(), "OpenGL context is not valid");

  //  Create our reading buffer
  auto readBuffer = make_shared<OpenGLTripleBuffer>(*this);

  m_captureContext = unique_ptr<ICaptureContext>( new CameraCapture( ) );
  m_captureContext->Init( readBuffer );
}

shared_ptr<QGLWidget> MainController::MakeSharedContext(void)
{
  shared_ptr<QGLWidget> sharedContext(nullptr);

  if(nullptr != m_mainContext)
  {
	sharedContext = shared_ptr<QGLWidget>( new QGLWidget( 0, m_mainContext.get( ) ) );
	Utils::AssertOrThrowIfFalse(sharedContext->isSharing( ), "Unable to create a shared OpenGL context" );
  }
  
  return sharedContext;
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