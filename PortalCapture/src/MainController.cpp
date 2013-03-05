#include "MainController.h"

MainController::MainController() : QObject(), m_processContext(nullptr)
{ }

void MainController::Init(QString initScriptFilename)
{
  // By creating a QGLWidget we will get a QGLContext. As long as we
  // dont show it we will have a headless GLContext. Athough headless
  // we must have an actual screen though (X Session or Explorer session)
  // so that we can get a window and graphics context.
  wrench::Logger::logDebug("Loading (Process) context");
  m_processContext = make_shared<PortalProcessor>( );  
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
  wrench::Logger::logDebug("Loading scripting interface");
  m_interface = shared_ptr<ScriptInterface>( new ScriptInterface() );
  m_interface->AddObject(shared_ptr<MainController>(this), "Main");
  m_interface->PushThis("Main");  // We are now the new 'this'
  m_interface->AddObject(m_interface, "Global");
  m_interface->AddObject(m_processContext, "Process");
  m_interface->RunScript(initScriptFilename);

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

void MainController::InitProcessContext( QString outputBufferName)
{
  auto outputBuffer = m_interface->ResolveObject<IWriteBuffer>(outputBufferName);
  
  m_processContext->Init( outputBuffer );
}

// ---------------------- Factory Methods for the scripting interface

void MainController::NewBuffer( QString bufferName, bool makeReadContext, bool makeWriteContext )
{
  // Make the buffer then add it to the scripting interface
  auto buffer = make_shared<OpenGLTripleBuffer>( this, makeReadContext, makeWriteContext );
  m_interface->AddObject(buffer, bufferName);
}

void MainController::NewMultiBuffer( QString bufferName, bool makeReadContext, bool makeWriteContext, int bufferCount)
{
  // Make the buffer then add it to the scripting interface
  auto buffer = make_shared<MultiOpenGLBuffer>( bufferCount, makeReadContext, makeWriteContext, this );
  m_interface->AddObject(buffer, bufferName);
}

void MainController::NewCaptureContext( QString contextName, QString outputBufferName )
{
  // Init our output context
  wrench::Logger::logDebug("Creating (%s) context", contextName.toLocal8Bit().data()); 
  auto buffer = m_interface->ResolveObject<IWriteBuffer>(outputBufferName);
  
  auto context = make_shared<CameraCapture>( buffer );
  m_contexts.insert( make_pair<QString, shared_ptr<IContext>>( contextName, context) );
	  
  // Add our object to the script interface
  m_interface->AddObject(context, contextName);
}

void MainController::NewSixFringeProcessor( QString contextName, QString inputBufferName, QString calibrationName )
{
  wrench::Logger::logDebug("Creating (%s) context", contextName.toLocal8Bit().data()); 
  auto buffer = m_interface->ResolveObject<MultiOpenGLBuffer>( inputBufferName );
  auto calibration = m_interface->ResolveObject<CalibrationData>( calibrationName );
  auto context = make_shared<SixFringeProcessor>(buffer, calibration);
  
  m_processContext->AddProcessContext(context);

  // Add our object to the script interface
  m_interface->AddObject(context, contextName);
}

void MainController::NewStreamContext( QString contextName, int port, QString inputBufferName )
{
  wrench::Logger::logDebug("Creating (%s) context", contextName.toLocal8Bit().data());  
  auto buffer = m_interface->ResolveObject<IReadBuffer>(inputBufferName);
 
  // Create our object and add to the list of contexts
  auto context = make_shared<WebsocketStream> ( port, buffer );
  m_contexts.insert( make_pair<QString, shared_ptr<IContext>>(contextName, context) );

  // Add our object to the script interface
  m_interface->AddObject(context, contextName);
}

void MainController::NewCalibrationData( QString calibrationObjectName, QString configScriptFilePath )
{
  wrench::Logger::logDebug( "Creating (%s) calibration data", calibrationObjectName.toLocal8Bit().data() );  
  
  // Create our calibration data and then set it to the new 'this' object
  auto calibrationData = make_shared<CalibrationData>( );
  m_interface->AddObject( calibrationData, calibrationObjectName );
  m_interface->PushThis( calibrationObjectName );
  m_interface->RunScript( configScriptFilePath );

  // Restore 'this' object once we are done
  m_interface->PopThis( );
}