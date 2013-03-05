#include "SixFringeProcessor.h"

SixFringeProcessor::SixFringeProcessor( void ) :
  m_isInit(false), m_captureReference(true), m_shift(0.0f), m_scale(1.0), m_outputTexture(&m_encodedMap), m_gaussFilter(11)
{ }

void SixFringeProcessor::AddCapture( shared_ptr<MultiOpenGLBuffer> inputBuffer, shared_ptr<CalibrationData> calibrationData )
{
  // Remember, due to move semantics this function will take ownership of the calibrationData
  m_captureBuffers.push_back(make_pair(inputBuffer, ::move(calibrationData)));
}

void SixFringeProcessor::Init( shared_ptr<IWriteBuffer> outputBuffer )
{
  Utils::AssertOrThrowIfFalse(nullptr != outputBuffer, "Invalid output buffer");
  Utils::AssertOrThrowIfFalse(0 < m_captureBuffers.size(), "Must have an input capture buffer");

  //  All the buffers should be the same size. Just grab the first one
  int width = m_captureBuffers[0].first->GetWidth();
  int height = m_captureBuffers[0].first->GetHeight();

  // Make sure we are the current OpenGL Context
  makeCurrent( );
  m_outputBuffer = outputBuffer;
  m_outputBuffer->InitWrite( width, height );

  // Make sure we are the current OpenGL Context again. 
  // Buffers might have allocated their own contexts
  makeCurrent( );

  //  Initialize our shaders
  m_fringe2Phase.init();
  m_fringe2Phase.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  m_fringe2Phase.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/Fringe2Phase.frag"));
  m_fringe2Phase.bindAttributeLocation("vert", 0);
  m_fringe2Phase.bindAttributeLocation("vertTexCoord", 1);
  m_fringe2Phase.link();
  m_fringe2Phase.uniform("fringeImage1", 0);
  m_fringe2Phase.uniform("fringeImage2", 1); 
  m_fringe2Phase.uniform("gammaCutoff", 0.0f);
  m_fringe2Phase.uniform("pitch1", 60.0f);
  m_fringe2Phase.uniform("pitch2", 63.0f);

  m_phaseFilter.init();
  m_phaseFilter.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  m_phaseFilter.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/PhaseFilter.frag"));
  m_phaseFilter.bindAttributeLocation("vert", 0);
  m_phaseFilter.bindAttributeLocation("vertTexCoord", 1);
  m_phaseFilter.link();
  m_phaseFilter.uniform("image", 0);
  //  In the shader these are floating point, so ensure that they are with a cast
  m_phaseFilter.uniform("width", ( float )width );
  m_phaseFilter.uniform("height", ( float )height );
	
  m_gaussFilter.init();
  m_gaussFilter.setImageDimensions( width, height );

  m_phase2Depth.init();
  m_phase2Depth.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  m_phase2Depth.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/Phase2Depth.frag"));
  m_phase2Depth.bindAttributeLocation("vert", 0);
  m_phase2Depth.bindAttributeLocation("vertTexCoord", 1);
  m_phase2Depth.link();
  m_phase2Depth.uniform("actualPhase", 0);
  m_phase2Depth.uniform("referencePhase", 1);
  m_phase2Depth.uniform("scale", m_scale);
  m_phase2Depth.uniform("shift", m_shift);
	
  m_depth2Holo.init();
  m_depth2Holo.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  m_depth2Holo.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/Depth2Holo.frag"));
  m_depth2Holo.link();
  m_depth2Holo.uniform("fringeFrequency", 16.0f);
  m_depth2Holo.uniform("depthMap", 0);

  m_renderTexture.init();
  m_renderTexture.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  m_renderTexture.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/RenderTexture.frag"));
  m_renderTexture.link();
  m_renderTexture.uniform("image", 0);

  m_phaseMap0.init		( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_phaseMap1.init		( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_referencePhase.init	( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_depthMap.init		( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_encodedMap.init		( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );

  m_imageProcessor.init( width, height );
  m_imageProcessor.setTextureAttachPoint( m_phaseMap0,		GL_COLOR_ATTACHMENT0 );
  m_imageProcessor.setTextureAttachPoint( m_phaseMap1,		GL_COLOR_ATTACHMENT1 );
  m_imageProcessor.setTextureAttachPoint( m_referencePhase,	GL_COLOR_ATTACHMENT2 );
  m_imageProcessor.setTextureAttachPoint( m_depthMap,		GL_COLOR_ATTACHMENT3 );
  m_imageProcessor.setTextureAttachPoint( m_encodedMap,		GL_COLOR_ATTACHMENT4 );
  m_imageProcessor.unbind( );

  m_isInit = true;
  OGLStatus::logOGLErrors("SixFringeProcessor - Init( shared_ptr<IOpenGLReadBuffer> )");
}

void SixFringeProcessor::CaptureReference( void )
{
  wrench::Logger::logDebug("Capturing a reference plane");
  m_captureReference = true;
}

void SixFringeProcessor::SetScale( float scale )
{
  m_scale = scale;
}

void SixFringeProcessor::SetShift( float shift )
{
  m_shift = shift;
}

void SixFringeProcessor::OutputFringe( void )
{
  m_outputTexture = nullptr;
}

void SixFringeProcessor::OutputDepth( void )
{
  m_outputTexture = &m_depthMap;
}

void SixFringeProcessor::OutputHolo( void )
{
  m_outputTexture = &m_encodedMap;
}

void SixFringeProcessor::paintGL( void )
{
  // If we are not init then just return
  if( !m_isInit )
	{ return; }

  OGLStatus::logOGLErrors("SixFringeProcessor - paintGL( )");

  //  Make sure we are the current OpenGL Context
  makeCurrent( );

  // TODO Comeback and fix this
  auto inputBuffer = m_captureBuffers[0].first;

  // This will swap to the newest read buffer
  for ( auto itr = inputBuffer->ReadBuffersBegin() ; itr != inputBuffer->ReadBuffersEnd( ); ++itr)
  { 
	(*itr)->StartRead();
  }

  // Our actual decoding is done here
  m_imageProcessor.bind();
  {
	_calculatePhase( GL_COLOR_ATTACHMENT0, inputBuffer );
	_filterPhase( GL_COLOR_ATTACHMENT1, m_phaseMap0 );

	//	If we are capturing reference phase, just filter into the reference phase
	if( m_captureReference )
	{ 
	  _filterPhase( GL_COLOR_ATTACHMENT2, m_phaseMap1 ); 
	  m_captureReference = false;
	}

	_calculateDepth( GL_COLOR_ATTACHMENT3, m_phaseMap1 );
	_holoEncode( GL_COLOR_ATTACHMENT4 );

	m_imageProcessor.setTextureAttachPoint( m_outputBuffer->StartWriteTexture( ), GL_COLOR_ATTACHMENT5 );
	_outputTexture( GL_COLOR_ATTACHMENT5 );
  }
  m_imageProcessor.unbind();

  m_outputBuffer->WriteFinished( );
}

void SixFringeProcessor::_calculatePhase(GLenum drawBuffer, shared_ptr<MultiOpenGLBuffer> fringeBuffer)
{
  m_imageProcessor.bindDrawBuffer( drawBuffer );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_fringe2Phase.bind( );

  //  Bind all our fringe images
  int texNumber = 0;
  for ( auto itr = fringeBuffer->ReadBuffersBegin() ; itr != fringeBuffer->ReadBuffersEnd( ); ++itr)
  { 
	(*itr)->ReadTexture().bind( GL_TEXTURE0 + texNumber++ );
  }
  m_imageProcessor.process( );
}

void SixFringeProcessor::_filterPhase( GLenum drawBuffer, Texture& phase2Filter )
{
  m_imageProcessor.bindDrawBuffer( drawBuffer );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_phaseFilter.bind( );
  phase2Filter.bind( GL_TEXTURE0 );
  m_imageProcessor.process( );
}

void SixFringeProcessor::_calculateDepth( GLenum drawBuffer, Texture& phase )
{
  m_imageProcessor.bindDrawBuffer( drawBuffer );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_phase2Depth.bind( );
  m_phase2Depth.uniform("scale", m_scale);
  m_phase2Depth.uniform("shift", m_shift);
  phase.bind( GL_TEXTURE0 );
  m_referencePhase.bind( GL_TEXTURE1 );
  m_imageProcessor.process( );
}

void SixFringeProcessor::_holoEncode( GLenum drawBuffer )
{
  m_imageProcessor.bindDrawBuffer( drawBuffer );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_depth2Holo.bind();
  m_depthMap.bind( GL_TEXTURE0 );
  m_imageProcessor.process( );
}

void SixFringeProcessor::_outputTexture( GLenum drawBuffer )
{
  m_imageProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT5 );
  m_renderTexture.bind( );
  // If our outputTexture is null then it means output the fringe
  if(nullptr == m_outputTexture)
  {

	// TODO - Comeback and revisit this
	//	Only need to bind the first one since at most we can only output 1 image
	auto itr = m_captureBuffers[0].first->ReadBuffersBegin();
	if ( itr != m_captureBuffers[0].first->ReadBuffersEnd() )
	{
	  (*itr)->ReadTexture().bind( GL_TEXTURE0 );
	}
  }
  else
	{ m_outputTexture->bind( GL_TEXTURE0 ); }

  m_imageProcessor.process( );
}