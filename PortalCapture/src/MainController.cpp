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
  m_processContext->makeCurrent( );
  m_processContext->updateGL( );

  //  Init GLEW so that we can make fancy OpenGL calls
  GLenum status = glewInit();
  Utils::AssertOrThrowIfFalse(GLEW_OK == status, "Failed to init GLEW:" );
  wrench::Logger::logDebug("Using GLEW Version: %s", glewGetString( GLEW_VERSION ));

  //  Create our capture buffer and processed buffers
  //  TODO: Need to know how we need 2 buffers
  auto captureBuffer	= make_shared<MultiOpenGLBuffer>( 2, this );
  auto processedBuffer	= make_shared<OpenGLTripleBuffer>( this, true, false );

  // ----- Initialize our contexts -----
  // Init our capture context
  m_captureContext = unique_ptr<ICaptureContext>( new CameraCapture( ) );
  m_captureContext->Init( captureBuffer );

  // Init our processing/main context
  m_processContext->Init( captureBuffer, processedBuffer );
  
  // Init our output context
  m_streamContext = unique_ptr<IStreamContext> ( new WebsocketStream( ) );
  m_streamContext->Init( processedBuffer );

  //  Wire up signals and slots
  connect(captureBuffer.get( ), SIGNAL( WriteFilled( ) ), m_processContext.get( ), SLOT( updateGL( ) ) );

  wrench::Logger::logDebug("Initialization complete");
}

shared_ptr<QGLWidget> MainController::MakeSharedContext(void)
{
  Utils::AssertOrThrowIfFalse(nullptr != m_processContext, "Need to have a main context to make the shared from");

  shared_ptr<QGLWidget> sharedContext( new QGLWidget( m_processContext.get( ), m_processContext.get( ) ) );

  //  Make sure that we created the context and that it is properly sharing
  Utils::AssertOrThrowIfFalse(sharedContext->isSharing( ), "Unable to create a shared OpenGL context" );
  Utils::AssertOrThrowIfFalse(QGLContext::areSharing( sharedContext->context( ), m_processContext->context( ) ), "Sharing between contexts failed" );
  
  return sharedContext;
}

void MainController::Start(void)
{
  wrench::Logger::logDebug("Started");
  m_captureContext->Start( );
  m_streamContext->Start( );
}

void MainController::Close(void)
{
  wrench::Logger::logDebug("Closing");
  //  This will tell the event loop that we are done and close the app
  emit( Finished( ) );
}