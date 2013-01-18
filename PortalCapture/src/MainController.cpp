#include "MainController.h"

MainController::MainController() : QObject(), m_processContext(nullptr), m_captureContext(nullptr)
{ }

void MainController::Init(void)
{
  // By creating a QGLWidget we will get a QGLContext. As long as we
  // dont show it we will have a headless GLContext. Athough headless
  // we must have an actual screen though (X Session or Explorer session)
  // so that we can get a window and graphics context.
  m_processContext = unique_ptr<SixFringeProcessor>( new SixFringeProcessor( ) );  
  Utils::AssertOrThrowIfFalse(m_processContext->isValid(), "OpenGL context is not valid");
  
  //  Calling updateGL will initialize our context so that 
  //  we can actually perform OpenGL calls
  m_processContext->updateGL();

  //  Init GLEW so that we can make fancy OpenGL calls
  GLenum status = glewInit();
  Utils::AssertOrThrowIfFalse(GLEW_OK == status, "Failed to init GLEW:" );
  wrench::Logger::logDebug("Using GLEW Version: %s", glewGetString( GLEW_VERSION ));
  cout << "Using GLEW Version: " << glewGetString( GLEW_VERSION ) << endl;

  //  Create our capture buffer and initialize our contexts
  //  TODO: Need to know how we need 2 buffers
  auto captureBuffer = make_shared<MultiOpenGLBuffer>(2, *this);
  m_captureContext = unique_ptr<ICaptureContext>( new CameraCapture( ) );
  m_captureContext->Init( captureBuffer );

  //  Init our processing/main context
  m_processContext->Init( captureBuffer );
}

shared_ptr<QGLContext> MainController::MakeSharedContext(void)
{
  shared_ptr<QGLContext> sharedContext(nullptr);

  if(nullptr != m_processContext)
  {
	sharedContext = shared_ptr<QGLContext>( new QGLContext(m_processContext->format(), m_processContext.get( ) ) );
	bool created = sharedContext->create( m_processContext->context( ) );
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