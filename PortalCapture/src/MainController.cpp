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
  
  //  Calling updateGL will initialize our context so that 
  //  we can actually perform OpenGL calls
  m_mainContext->updateGL();

  //  Init GLEW so that we can make fancy OpenGL calls
  GLenum status = glewInit();
  Utils::AssertOrThrowIfFalse(GLEW_OK == status, "Failed to init GLEW:" );
  wrench::Logger::logDebug("Using GLEW Version: %s", glewGetString( GLEW_VERSION ));
  cout << "Using GLEW Version: " << glewGetString( GLEW_VERSION ) << endl;

  //  Create our reading buffer and initialize our contexts
  auto readBuffer = make_shared<OpenGLTripleBuffer>(*this);
  m_captureContext = unique_ptr<ICaptureContext>( new CameraCapture( ) );
  m_captureContext->Init( readBuffer );
}

shared_ptr<QGLContext> MainController::MakeSharedContext(void)
{
  shared_ptr<QGLContext> sharedContext(nullptr);

  if(nullptr != m_mainContext)
  {
	sharedContext = shared_ptr<QGLContext>( new QGLContext(m_mainContext->format(), m_mainContext.get( ) ) );
	bool created = sharedContext->create( m_mainContext->context( ) );
	Utils::AssertOrThrowIfFalse(created, "Unable to create a shared OpenGL context" );
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