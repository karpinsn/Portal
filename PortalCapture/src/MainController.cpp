#include "MainController.h"

MainController::MainController( shared_ptr<ScriptInterface> scriptInterface ) : 
  QObject( ), m_processContext(nullptr), m_interface(scriptInterface)
{ }

void MainController::Init(QString initScriptFilename)
{
  // By creating a QGLWidget we will get a QGLContext. As long as we
  // dont show it we will have a headless GLContext. Athough headless
  // we must have an actual screen though (X Session or Explorer session)
  // so that we can get a window and graphics context.
  wrench::Logger::logDebug("Loading (Process) context");
  m_processContext = make_shared<PortalProcessor>( );  
  Utils::ThrowIfFalse(m_processContext->isValid(), "OpenGL context is not valid");
  
  //  Calling updateGL will initialize our context so that 
  //  we can actually perform OpenGL calls
  m_processContext->makeCurrent( );
  m_processContext->updateGL( );

  //  Init GLEW so that we can make fancy OpenGL calls
  GLenum status = glewInit();
  Utils::ThrowIfFalse(GLEW_OK == status, "Failed to init GLEW" );
  wrench::Logger::logDebug("Using GLEW Version: %s", glewGetString( GLEW_VERSION ));

  //  Now that we are initalized (OpenGL, GLEW, etc) we can run our init script
  wrench::Logger::logDebug("Loading scripting interface");
  
  // If we have a scripting interface then add ourselves and run the init script
  if(nullptr != m_interface)
  {
	m_interface->AddObject(shared_ptr<MainController>(this), "Main");
	m_interface->PushThis("Main");  // We are now the new 'this'
	m_interface->AddObject(m_interface, "Global");
	m_interface->AddObject(m_processContext, "Process");
	m_interface->RunScript(initScriptFilename);
  }

  wrench::Logger::logDebug("Initialization complete");
}

shared_ptr<QGLWidget> MainController::MakeSharedContext(void)
{
  Utils::ThrowIfFalse(nullptr != m_processContext, "Need to have a main context to make the shared from");
  shared_ptr<QGLWidget> sharedContext( new QGLWidget( m_processContext.get( ), m_processContext.get( ) ) );

  //  Make sure that we created the context and that it is properly sharing
  Utils::ThrowIfFalse(sharedContext->isSharing( ), "Unable to create a shared OpenGL context" );
  Utils::ThrowIfFalse(QGLContext::areSharing( sharedContext->context( ), m_processContext->context( ) ), "Sharing between contexts failed" );
  
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

void MainController::InitProcessContext( QString outputBufferName )
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

void MainController::NewCamera( QString cameraName, QString cameraType, QString configScript )
{
  shared_ptr<lens::ICamera> camera;

  if( 0 == cameraType.compare( QString("OpenCV") ) )
  {
	camera = make_shared<lens::OpenCVCamera>( );
  }
  // Add possibilities of more cameras based on config
  #ifdef USE_FILE_CAMERA
  else if( 0 == cameraType.compare( QString("FileCamera") ) )
  {
	camera = make_shared<lens::FileCamera>( );
  }
  #endif
  #ifdef USE_POINT_GREY_CAMERA
  else if( 0 == cameraType.compare( QString("PointGrey") ) )
  {
	//	Create a point grey camera and instance it
	camera = make_shared<lens::PointGreyCamera>( );
  }
  #endif //USE_POINT_GREY_CAMERA
 
  Utils::ThrowIfFalse(nullptr != camera, "Unknown camera requested, unable to instantiate");

  if(nullptr != m_interface)
  {
	m_interface->AddObject( camera, cameraName );

	if( !configScript.isNull( ) && !configScript.isEmpty( ) )
	{
	  m_interface->PushThis( cameraName );
	  m_interface->RunScript( configScript );

	  // Restore 'this' object once we are done
	  m_interface->PopThis( );
	}
  }
}

void MainController::NewCaptureContext( QString contextName, QString cameraName, QString outputBufferName )
{
  // Init our output context
  wrench::Logger::logDebug("Creating (%s) context", contextName.toLocal8Bit().data()); 
  auto buffer = m_interface->ResolveObject<IWriteBuffer>( outputBufferName );
  auto camera = m_interface->ResolveObject<lens::ICamera>( cameraName );

  auto context = make_shared<CameraCapture>( buffer, camera );
  m_contexts.insert( make_pair( contextName, context) );
	  
  // Add our object to the script interface
  m_interface->AddObject(context, contextName);
}

void MainController::NewSixFringeProcessor( QString contextName, QString inputBufferName, QString cameraCalibrationName, QString projectorCalibrationName )
{
  wrench::Logger::logDebug("Creating (%s) context", contextName.toLocal8Bit().data()); 
  auto buffer = m_interface->ResolveObject<MultiOpenGLBuffer>( inputBufferName );
  auto cameraCalibration = m_interface->ResolveObject<CalibrationData>( cameraCalibrationName );
  auto projectorCalibration = m_interface->ResolveObject<CalibrationData>( projectorCalibrationName );
  auto context = make_shared<SixFringeProcessor>(buffer, cameraCalibration, projectorCalibration);
  
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
  m_contexts.insert( make_pair(contextName, context) );

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