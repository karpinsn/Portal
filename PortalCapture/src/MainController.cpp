#include "MainController.h"

MainController::MainController() : QObject(), m_processContext(nullptr), m_captureContext(nullptr)
{ }

void MainController::Init(QString initScriptFilename)
{
  // By creating a QGLWidget we will get a QGLContext. As long as we
  // dont show it we will have a headless GLContext. Athough headless
  // we must have an actual screen though (X Session or Explorer session)
  // so that we can get a window and graphics context.
  wrench::Logger::logDebug("Loading (Process) context");
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
  m_interface->AddObject(m_interface.get(), "Global");
  m_interface->AddObject(m_processContext.get(), "Process");

  m_interface->RunScript(initScriptFilename);
  wrench::Logger::logDebug("Loading scripting interface");

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
  // Make the buffer then add it to the scripting interface
  auto buffer = make_shared<OpenGLTripleBuffer>( this, makeReadContext, makeWriteContext );
  m_buffers.insert( pair<QString, shared_ptr<ITripleBuffer>>(
	bufferName, buffer ) );

  m_interface->AddObject(buffer.get(), bufferName);
}

void MainController::AddMultiBuffer( QString bufferName, int bufferCount)
{
  // Make the buffer then add it to the scripting interface
  auto buffer = make_shared<MultiOpenGLBuffer>( bufferCount, this );
  m_buffers.insert( pair<QString, shared_ptr<ITripleBuffer>>(
	bufferName, buffer ) );

  m_interface->AddObject(buffer.get(), bufferName);
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

void MainController::InitProcessContext( QString inputBufferName, QString outputBufferName)
{
  auto inputBuffer = m_buffers.at(inputBufferName);
  auto outputBuffer = m_buffers.at(outputBufferName);
  Utils::AssertOrThrowIfFalse(nullptr != inputBuffer, "Unknown buffer");
  Utils::AssertOrThrowIfFalse(nullptr != outputBuffer, "Unknown buffer");

  m_processContext->Init( inputBuffer, outputBuffer );
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
  for(auto context = m_contexts.begin(); context != m_contexts.end(); ++context)
  {
	context->second->Start( );
  }
  wrench::Logger::logDebug("Started");
}