#include "MainController.h"

MainController::MainController() : QObject(), m_processContext(nullptr), m_captureContext(nullptr)
{ }

void MainController::Init(QString initScriptFilename)
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

  //  Now that we are initalized (OpenGL, GLEW, etc) we can run our init script
  m_interface = unique_ptr<ScriptInterface>( new ScriptInterface() );
  m_interface->AddObject(this, "Main");

  m_interface->RunScript(initScriptFilename);

  //  Create our capture buffer and processed buffers
  //  TODO: Need to know how we need 2 buffers
  //auto captureBuffer	= make_shared<MultiOpenGLBuffer>( 2, this );

  // ----- Initialize our contexts -----
  // Init our capture context
  //wrench::Logger::logDebug("Loading (Capture) context");
  //  TODO: Unsigned int for camera serial number
  //m_captureContext = unique_ptr<ICaptureContext>( new CameraCapture( ) );
  //m_captureContext->Init( captureBuffer );

  // Init our processing/main context

  wrench::Logger::logDebug("Loading (Process) context");
  auto processedBuffer = m_buffers.at("StreamBuffer");
  auto captureBuffer = m_buffers.at("CaptureBuffer");
  m_processContext->Init( captureBuffer, processedBuffer );
  
  //  Wire up signals and slots
  connect(captureBuffer.get( ), SIGNAL( WriteFilled( ) ), m_processContext.get( ), SLOT( updateGL( ) ) );

  wrench::Logger::logDebug("Loading scripting interface");
  //  Add the scripting interface so that we can communicate with the app
  m_interface->AddObject(m_captureContext.get(), "Capture");
  m_interface->AddObject(m_processContext.get(), "Process");
  //
  
  

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
  // This will put start on top of the event loop
  QTimer::singleShot(0, this, SLOT( StartSystem() ));
}

void MainController::AddBuffer( QString bufferName, bool makeReadContext, bool makeWriteContext )
{
  m_buffers.insert( pair<QString, shared_ptr<ITripleBuffer>>(
	bufferName, make_shared<OpenGLTripleBuffer>( this, makeReadContext, makeWriteContext ) ) );
}

void MainController::AddMultiBuffer( QString bufferName, int bufferCount)
{
  m_buffers.insert( pair<QString, shared_ptr<ITripleBuffer>>(
	bufferName, make_shared<MultiOpenGLBuffer>( bufferCount, this ) ) );
}

void MainController::AddCaptureContext( QString contextName, QString outputBufferName )
{
    // Init our output context
  wrench::Logger::logDebug("Loading (%s) context", contextName.toLocal8Bit().data()); 

  auto context = make_shared<CameraCapture>( );
  m_contexts.insert(
	pair<QString, shared_ptr<IContext>>(
	  contextName, context) );

  auto buffer = m_buffers.at(outputBufferName);
  Utils::AssertOrThrowIfFalse(nullptr != buffer, "Unknown buffer");
  context->Init(buffer);

  // Add our object to the script interface
  m_interface->AddObject(context.get(), contextName);
}

void MainController::AddStreamContext( QString contextName, QString inputBufferName )
{
  // Init our output context
  wrench::Logger::logDebug("Loading (%s) context", contextName.toLocal8Bit().data());  
  
  // Create our object and add to the list of contexts
  auto context = make_shared<WebsocketStream> ( );
  m_contexts.insert(
	pair<QString, shared_ptr<IContext>>(
	  contextName, context) );
  
  // Init with the input buffer
  auto buffer = m_buffers.at(inputBufferName);
  Utils::AssertOrThrowIfFalse(nullptr != buffer, "Unknown buffer");
  context->Init(buffer);

  // Add our object to the script interface
  m_interface->AddObject(context.get(), contextName);
}

void MainController::Close(void)
{
  wrench::Logger::logDebug("Closing");
  //  This will tell the event loop that we are done and close the app
  emit( Finished( ) );
}

void MainController::StartSystem(void)
{
  m_captureContext->Start( );
  //m_streamContext->Start( );
  wrench::Logger::logDebug("Started");
}