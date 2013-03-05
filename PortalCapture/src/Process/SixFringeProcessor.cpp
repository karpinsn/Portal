#include "SixFringeProcessor.h"

SixFringeProcessor::SixFringeProcessor( shared_ptr<MultiOpenGLBuffer> inputBuffer, shared_ptr<CalibrationData> calibrationData ) :
  m_isInit(false), m_captureReference(true), 
  m_shift(0.0f), m_scale(.01f), 
  m_gaussFilter(11), m_inputBuffer(inputBuffer), 
  m_calibrationData(calibrationData)
{ }

void SixFringeProcessor::Init( )
{
  //  All the buffers should be the same size. Just grab the first one
  int width = m_inputBuffer->GetWidth();
  int height = m_inputBuffer->GetHeight();

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

  m_phaseMap0.init		( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_phaseMap1.init		( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_referencePhase.init	( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_depthMap.init		( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );

  m_imageProcessor.init( width, height );
  m_imageProcessor.setTextureAttachPoint( m_phaseMap0,		GL_COLOR_ATTACHMENT0 );
  m_imageProcessor.setTextureAttachPoint( m_phaseMap1,		GL_COLOR_ATTACHMENT1 );
  m_imageProcessor.setTextureAttachPoint( m_referencePhase,	GL_COLOR_ATTACHMENT2 );
  m_imageProcessor.setTextureAttachPoint( m_depthMap,		GL_COLOR_ATTACHMENT3 );
  m_imageProcessor.unbind( );

  m_isInit = true;
  OGLStatus::logOGLErrors("SixFringeProcessor - Init( shared_ptr<IOpenGLReadBuffer> )");
}

void SixFringeProcessor::BindDepthMap( GLenum texture )
{
  m_depthMap.bind(texture);
}

void SixFringeProcessor::BindFringeImage( GLenum texture )
{
  ( *m_inputBuffer->ReadBuffersBegin( ) )->ReadTexture( ).bind( texture );
}

void SixFringeProcessor::SetScale( float scale )
{
  m_scale = scale;
}

void SixFringeProcessor::SetShift( float shift )
{
  m_shift = shift;
}

void SixFringeProcessor::Process( void )
{
  // If we are not init then just return
  if( !m_isInit )
	{ return; }

  OGLStatus::logOGLErrors("SixFringeProcessor - Process( )");

  for( auto itr = m_inputBuffer->ReadBuffersBegin(); itr != m_inputBuffer->ReadBuffersEnd(); ++itr )
  {
	(*itr)->StartRead();
  }

  // Our actual decoding is done here
  m_imageProcessor.bind();
  {
	_calculatePhase( GL_COLOR_ATTACHMENT0, m_inputBuffer );
	_filterPhase( GL_COLOR_ATTACHMENT1, m_phaseMap0 );
	_calculateDepth( GL_COLOR_ATTACHMENT3, m_phaseMap1 );
  }
  m_imageProcessor.unbind();
}

void SixFringeProcessor::CaptureReference( void )
{
  // If we are not init then just return
  if( !m_isInit )
	{ return; }

  OGLStatus::logOGLErrors("SixFringeProcessor - Process( )");

  for( auto itr = m_inputBuffer->ReadBuffersBegin(); itr != m_inputBuffer->ReadBuffersEnd(); ++itr )
  {
	(*itr)->StartRead();
  }

  // Our actual decoding is done here
  m_imageProcessor.bind();
  {
	_calculatePhase( GL_COLOR_ATTACHMENT0, m_inputBuffer );
	_filterPhase( GL_COLOR_ATTACHMENT1, m_phaseMap0 );
	_filterPhase( GL_COLOR_ATTACHMENT2, m_phaseMap1 ); 
	m_captureReference = false;
  }
  m_imageProcessor.unbind();
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