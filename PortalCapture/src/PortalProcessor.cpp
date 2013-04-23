#include "PortalProcessor.h"

PortalProcessor::PortalProcessor( void ) :
  m_isInit(false), m_displayMode(Holo), m_displayNumber(0)
{ }

void PortalProcessor::AddProcessContext( shared_ptr<IProcessContext> processContext )
{
  m_captureProcessors.push_back( make_pair( processContext, make_shared<TriMesh>( processContext->GetWidth( ), processContext->GetHeight( ) ) ) );
}

void PortalProcessor::Init( shared_ptr<IWriteBuffer> outputBuffer )
{
  Utils::AssertOrThrowIfFalse(nullptr != outputBuffer, "Invalid output buffer");
  Utils::AssertOrThrowIfFalse(0 < m_captureProcessors.size(), "Must have an input capture buffer");

  // TODO - Fix this
  int width = 800;
  int height = 600;

  // Make sure we are the current OpenGL Context
  makeCurrent( );
  m_outputBuffer = outputBuffer;
  m_outputBuffer->InitWrite( width, height );

  // Make sure we are the current OpenGL Context again. 
  // Buffers might have allocated their own contexts
  makeCurrent( );

  // Init all of our processors
  for ( auto itr = m_captureProcessors.begin( ); itr != m_captureProcessors.end( ); ++itr )
  {
	(*itr).first->Init( );
	(*itr).second->initMesh( );
  }

  //  Initialize our shaders	
  m_coordinate2Holo.init();
  m_coordinate2Holo.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/Coordinate2Holo.vert"));
  m_coordinate2Holo.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/Coordinate2Holo.frag"));
  m_coordinate2Holo.link();
  m_coordinate2Holo.uniform("fringeFrequency", 16.0f);
  m_coordinate2Holo.uniform("coordinateMap", 0);

  // Define the matricies that we need for Holovideo encoding
  // --------------------------------------------------------
  // Scaling so that we fit in our cube around the origin
  glm::mat4 modelView = glm::scale( glm::mat4( ), glm::vec3( 1.0/1000.0f ) );
  m_coordinate2Holo.uniform( "modelView", modelView );
  // Rotate the projector modelView some angle around our model view (30 degrees?)
  glm::mat4 projectorModelView = glm::rotate( modelView, 30.0f, glm::vec3( 0.0f, 1.0f, 0.0f ) ) ;
  m_coordinate2Holo.uniform( "projectorModelView", projectorModelView );
  // Our projection is a cube around the origin
  glm::mat4 projection = glm::ortho( -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f );
  m_coordinate2Holo.uniform( "projectionMatrix", projection );

  m_renderTexture.init();
  m_renderTexture.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  m_renderTexture.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/RenderTexture.frag"));
  m_renderTexture.link();
  m_renderTexture.uniform("image", 0);

  m_encodedMap.init		( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );

  m_imageProcessor.init( width, height );
  m_imageProcessor.setTextureAttachPoint( m_encodedMap,	GL_COLOR_ATTACHMENT0 );
  m_imageProcessor.unbind( );

  m_isInit = true;
  OGLStatus::logOGLErrors("PortalProcessor - Init( shared_ptr<IOpenGLReadBuffer> )");
}

void PortalProcessor::OutputFringe( int processorNumber )
{
  if(processorNumber > (int)m_captureProcessors.size())
  {
	wrench::Logger::logDebug("Invalid processor number, defaulting to 0");
	m_displayNumber = processorNumber;
  }
  else
  {
	m_displayNumber = 0;
  }

  m_displayMode = Fringe;
}

void PortalProcessor::OutputDepth( int processorNumber )
{
  if(processorNumber > (int)m_captureProcessors.size())
  {
	wrench::Logger::logDebug("Invalid processor number, defaulting to 0");
	m_displayNumber = processorNumber;
  }
  else
  {
	m_displayNumber = 0;
  }

  m_displayMode = Depth;
}

void PortalProcessor::OutputHolo( void )
{
  m_displayMode = Holo;
}

void PortalProcessor::paintGL( void )
{
  // If we are not init then just return
  if( !m_isInit )
	{ return; }

  OGLStatus::logOGLErrors("PortalProcessor - paintGL( )");

  //  Make sure we are the current OpenGL Context
  makeCurrent( );
  _Process( );
}

void PortalProcessor::_Process( void )
{
  OGLStatus::logOGLErrors("PortalProcessor - _ProcessInput( )");
  for ( auto itr = m_captureProcessors.begin( ); itr != m_captureProcessors.end( ); ++itr )
  {
	(*itr).first->Process( );
  }

  // Now that we have processed, we need to rectify everything into one scan
  m_imageProcessor.bind();
  {
	m_imageProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT0 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_coordinate2Holo.bind();

	// For each processor, render it in the scene
	for ( int processor = 0; processor < (int)m_captureProcessors.size(); ++processor )
	{ 
	  m_captureProcessors[processor].first->BindDepthMap( GL_TEXTURE0 ); 
	  m_captureProcessors[processor].second->draw( );
	}

	// Now output whatever is selected as the output
	_Output( );
  }
  m_imageProcessor.unbind();

  m_outputBuffer->WriteFinished( );
}

void PortalProcessor::_Output( void )
{
  // Output results
  m_imageProcessor.setTextureAttachPoint( m_outputBuffer->StartWriteTexture( ), GL_COLOR_ATTACHMENT1 );
  m_imageProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT1 );
  m_renderTexture.bind( );

  switch (m_displayMode)
  {
	case Fringe:
	  m_captureProcessors[m_displayNumber].first->BindFringeImage( GL_TEXTURE0 );
	  break;
	case Depth:
	  m_captureProcessors[m_displayNumber].first->BindDepthMap( GL_TEXTURE0 );
	  break;
	case Holo:
	  m_encodedMap.bind( GL_TEXTURE0 );
	  break;
  }
  
  m_imageProcessor.process( );
}