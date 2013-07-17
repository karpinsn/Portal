#include "PortalProcessor.h"

PortalProcessor::PortalProcessor( void ) :
  m_isInit(false), m_displayMode(Holo), m_displayNumber(0)
{ }

void PortalProcessor::AddProcessContext( IProcessContext* processContext )
{
  m_captureProcessors.push_back( make_pair( processContext, nullptr ) );
}

void PortalProcessor::Init( ITripleBuffer* outputBuffer )
{
  Utils::ThrowIfFalse(nullptr != outputBuffer, "Invalid output buffer");
  Utils::ThrowIfFalse(0 < m_captureProcessors.size(), "Must have an input capture buffer");

  int width = ResolveProperty<int>( "outputWidth" );
  int height = ResolveProperty<int>( "outputHeight" );

  // Make sure we are the current OpenGL Context
  makeCurrent( );
  m_outputBuffer = outputBuffer;
  m_outputBuffer->InitWrite( width, height );

  // Make sure we are the current OpenGL Context again. 
  // Buffers might have allocated their own contexts
  makeCurrent( );

  // Init all of our processors and give them a splat field for rendering
  for ( auto itr = m_captureProcessors.begin( ); itr != m_captureProcessors.end( ); ++itr )
  {
	(*itr).first->Init( );
	(*itr).second = make_shared<SplatField>( (*itr).first->GetWidth(), (*itr).first->GetHeight() );
  }

  // Initialize our matricies -----------------------------------------------------
  // Scaling so that we fit in our cube around the origin
  glm::mat4 modelView = glm::scale( glm::mat4( ), glm::vec3( 1.0/500.0f ) );
  // Rotate the projector modelView some angle around our model view (30 degrees?)
  glm::mat4 projectorModelView = glm::rotate( modelView, 30.0f, glm::vec3( 0.0f, 1.0f, 0.0f ) ) ;
  // Our projection is a cube around the origin
  glm::mat4 projection = glm::ortho( -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f );

  // Init the OpenGL state that we need --------------------------------------------
  glEnable( GL_PROGRAM_POINT_SIZE );
  glEnable( GL_DEPTH_TEST );

  // Initialize our shaders --------------------------------------------------------
  // Shaders Pass 1 - Rectification: Depth
  m_coordinateRectifierPass1.init( );
  m_coordinateRectifierPass1.attachShader( new Shader( GL_VERTEX_SHADER, "Shaders/CoordinateRectifier.vert" ) );
  m_coordinateRectifierPass1.attachShader( new Shader( GL_FRAGMENT_SHADER, "Shaders/CoordinateRectifierDepth.frag") );
  m_coordinateRectifierPass1.link( );
  m_coordinateRectifierPass1.uniform( "coordinateMap", 0 );
  m_coordinateRectifierPass1.uniform( "modelView", modelView );
  m_coordinateRectifierPass1.uniform( "projectionMatrix", projection );
  m_coordinateRectifierPass1.uniform( "pointSize", ResolveProperty<float>( "pointSize" ) );
  m_coordinateRectifierPass1.uniform( "delta", .1f );

  // Shaders Pass 2 - Rectification: Splatting
  m_coordinateRectifierPass2.init( );
  m_coordinateRectifierPass2.attachShader( new Shader( GL_VERTEX_SHADER, "Shaders/CoordinateRectifier.vert" ) );
  m_coordinateRectifierPass2.attachShader( new Shader( GL_FRAGMENT_SHADER, "Shaders/CoordinateRectifierSplat.frag" ) );
  m_coordinateRectifierPass2.link( );
  m_coordinateRectifierPass2.uniform( "coordinateMap", 0 );
  m_coordinateRectifierPass2.uniform( "blendMap", 1 );
  m_coordinateRectifierPass2.uniform( "depthMap", 2 );
  m_coordinateRectifierPass2.uniform( "width", width );
  m_coordinateRectifierPass2.uniform( "height", height );
  m_coordinateRectifierPass2.uniform( "modelView", modelView );
  m_coordinateRectifierPass2.uniform( "projectionMatrix", projection );
  m_coordinateRectifierPass2.uniform( "pointSize", ResolveProperty<float>( "pointSize" ) );

  // Shader Pass 2 - Encoding
  m_coordinate2Holo.init( );
  m_coordinate2Holo.attachShader( new Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert" ) );
  m_coordinate2Holo.attachShader( new Shader(GL_FRAGMENT_SHADER, "Shaders/Coordinate2Holo.frag" ) );
  m_coordinate2Holo.link( );
  m_coordinate2Holo.uniform( "fringeFrequency", ResolveProperty<float>( "fringeFrequency" ) / 2.0f );
  m_coordinate2Holo.uniform( "coordinateMap", 0 );
  m_coordinate2Holo.uniform( "projectorModelView", projectorModelView );
  m_coordinate2Holo.uniform( "projectionMatrix", projection );

  // Shader Pass 3 - Final output
  m_renderTexture.init();
  m_renderTexture.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  m_renderTexture.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/RenderTexture.frag"));
  m_renderTexture.link();
  m_renderTexture.uniform("image", 0);

  // Initialize our textures --------------------------------------------------------
  auto blendImage = cv::imread("blend.png");
  m_blendMap.init( blendImage.cols, blendImage.rows, GL_RGBA, GL_RGB, GL_UNSIGNED_BYTE );
  assert( m_blendMap.transferToTexture( blendImage ) );

  m_rectifiedDepthMap.init( width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT );
  m_rectifiedCoordinateMap.init( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_encodedMap.init( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );

  m_imageProcessor.init( width, height );
  glViewport( 0, 0, width, height );

  m_imageProcessor.setTextureAttachPoint( m_rectifiedCoordinateMap, GL_COLOR_ATTACHMENT0 );
  m_imageProcessor.setTextureAttachPoint( m_encodedMap,	GL_COLOR_ATTACHMENT1 );
  m_imageProcessor.unbind( );

  m_isInit = true;
  OGLStatus::logOGLErrors("PortalProcessor - Init( shared_ptr<IOpenGLReadBuffer> )");
}

void PortalProcessor::OutputFringe( int processorNumber )
{
  // Check if we have an invalid processor number
  if(processorNumber > (int)m_captureProcessors.size() || processorNumber < 0)
  {
	wrench::Logger::logDebug("Invalid processor number, defaulting to 0");
	m_displayNumber = 0;
  }
  else
	{	m_displayNumber = processorNumber; }

  m_displayMode = Fringe;
}

void PortalProcessor::OutputDepth( )
  { m_displayMode = Depth; } 

void PortalProcessor::OutputCoord( )
  { m_displayMode = Coord; }

void PortalProcessor::OutputHolo( void )
  { m_displayMode = Holo; }

void PortalProcessor::paintGL( void )
{
  // If we are not init then just return
  if( !m_isInit )
	{ return; }

  OGLStatus::logOGLErrors("PortalProcessor - paintGL( )");

  makeCurrent( );                   //  Make sure we are the current OpenGL Context
  _Process( );                      // Process all of our capture processors
  _Output( );                       // Now output whatever is selected as the output
  m_outputBuffer->WriteFinished( ); // Inform the output buffer we are done and can swap
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
	// Shader pass 1 - Rectification: Depth
	// We need to render to the depth buffer first so that we can figure out if we need
	// to cull geometry in the second pass (Actual Splatting)
	m_coordinateRectifierPass1.bind( );
	m_imageProcessor.setTextureAttachPoint( m_rectifiedDepthMap, GL_DEPTH_ATTACHMENT ); // Make the depth render to our texture
	_RenderProcessors( );
	m_imageProcessor.resetDepthBuffer( ); // Set the depth buffer back so that we dont erase the data

	// Shader pass 2 - Rectification: Splatting
	// This is the actual splatting pass that will render out the rectified coordinate map
	// via splatting of the coordinates
	m_coordinateRectifierPass2.bind( );
	m_blendMap.bind( GL_TEXTURE1 );
	m_rectifiedDepthMap.bind( GL_TEXTURE2 );
	glDisable( GL_DEPTH_TEST ); // Disable depth test for this pass, we will manually do it
	_RenderProcessors( );
	glEnable( GL_DEPTH_TEST ); // Now put it back

	// Shader pass 2 - Encoding
	m_coordinate2Holo.bind( );
	m_imageProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT1 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	m_rectifiedCoordinateMap.bind( GL_TEXTURE0 );
	m_imageProcessor.process( );
  }
  m_imageProcessor.unbind();
}

void PortalProcessor::_Output( void )
{
  // Output results
  m_imageProcessor.bind();
  {
	m_imageProcessor.setTextureAttachPoint( m_outputBuffer->StartWriteTexture( ), GL_COLOR_ATTACHMENT2 );
	m_imageProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT2 );
	m_renderTexture.bind( );

	switch (m_displayMode)
	{
	  case Fringe:
		m_captureProcessors[m_displayNumber].first->BindFringeImage( GL_TEXTURE0 );
		break;
	  case Depth:
		m_rectifiedDepthMap.bind( GL_TEXTURE0 );
		break;
	  case Coord:
		m_rectifiedCoordinateMap.bind( GL_TEXTURE0 );
		break;
	  case Holo:
		m_encodedMap.bind( GL_TEXTURE0 );
		break;
	}
	
	// Now process into the output buffer
	m_imageProcessor.process( );
  }
}

void PortalProcessor::_RenderProcessors( )
{
  m_imageProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT0 );
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Make sure we clear to transparent
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // For each processor, render it in the scene
  for ( int processor = 0; processor < (int)m_captureProcessors.size(); ++processor )
  { 
	m_captureProcessors[processor].first->BindCoordMap( GL_TEXTURE0 ); 
	m_captureProcessors[processor].second->draw( );
  }
}