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
	
	// Register any types we need to pass around
	m_interface->RegisterMetaObjectType<lens::ICamera>( );
	m_interface->RegisterMetaObjectType<ITripleBuffer>( );
	m_interface->RegisterMetaObjectType<CalibrationData>( );
	m_interface->RegisterMetaObjectType<MainController>( );
	m_interface->RegisterMetaObjectType<OpenGLTripleBuffer>( );
	m_interface->RegisterMetaObjectType<MultiOpenGLBuffer>( );

	// Add our object types
	m_interface->AddObjectType<CalibrationData>( "CalibrationData" );
	qRegisterMetaType<MainController*>("MainController");
	m_interface->AddObjectType<OpenGLTripleBuffer, MainController*, bool, bool>( "OpenGLTripleBuffer" );
	m_interface->AddObjectType<MultiOpenGLBuffer, int, bool, bool, MainController*>( "MultiOpenGLBuffer" );
	m_interface->AddObjectType<CameraCapture, ITripleBuffer*, lens::ICamera*>( "CameraCapture" );
	m_interface->AddObjectType<SixFringeProcessor, MultiOpenGLBuffer*, CalibrationData*, CalibrationData*>( "SixFringeProcessor" );
	m_interface->AddObjectType<WebsocketStream, int, ITripleBuffer*>( "WebsocketStream" );

	// LensTypes
	m_interface->AddObjectType<lens::OpenCVCamera>( "OpenCVCamera" );
	#ifdef USE_FILE_CAMERA
	m_interface->AddObjectType<lens::FileCamera>( "FileCamera" );
	#endif
	#ifdef USE_IC_CAMERA
	m_interface->AddObjectType<lens::ICCamera>( "ICCamera" );
	#endif
	#ifdef USE_JAI_CAMERA
	m_interface->AddObjectType<lens::JAICamera>( "JAICamera" );
	#endif
	#ifdef USE_POINT_GREY_CAMERA
	m_interface->AddObjectType<lens::PointGreyCamera>( "PointGreyCamera" );
	m_interface->AddObjectType<lens::PointGreyCamera, unsigned int>( "PointGreyCamera" );
	#endif
	#ifdef USE_PHANTOM_CAMERA
	m_interface->AddObjectType<lens::PhantomCamera>( "PhantomCamera" );
	#endif
	
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
  Utils::ThrowIfFalse(QGLContext::areSharing( sharedContext->context( ), ( m_processContext)->QGLWidget::context( ) ), "Sharing between contexts failed" );
  
  return sharedContext;
}

void MainController::Start(void)
{ 
  wrench::Logger::logDebug("Started!");
  emit( Started( ) );
}

void MainController::Close(void)
{
  wrench::Logger::logDebug("Closing");
  //  This will tell the event loop that we are done and close the app
  emit( Finished( ) );
}