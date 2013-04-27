#include "SixFringeProcessor.h"

SixFringeProcessor::SixFringeProcessor( shared_ptr<MultiOpenGLBuffer> inputBuffer, shared_ptr<CalibrationData> cameraCalibration, shared_ptr<CalibrationData> projectorCalibration ) :
  m_isInit(false), m_gaussFilter(11), m_inputBuffer(inputBuffer), 
  m_cameraCalibration(cameraCalibration), m_projectorCalibration(projectorCalibration)
{ }

void SixFringeProcessor::Init( )
{
  //  All the buffers should be the same size. Just grab the first one
  int width = m_inputBuffer->GetWidth( );
  int height = m_inputBuffer->GetHeight( );

  //  Initialize our shaders
  m_fringe2Phase.init();
  m_fringe2Phase.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  m_fringe2Phase.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/Fringe2WrappedPhase.frag"));
  m_fringe2Phase.bindAttributeLocation("vert", 0);
  m_fringe2Phase.bindAttributeLocation("vertTexCoord", 1);
  m_fringe2Phase.link();
  m_fringe2Phase.uniform("fringeImage1", 0);
  m_fringe2Phase.uniform("fringeImage2", 1); 
  m_fringe2Phase.uniform("gammaCutoff", 0.0f);
  m_fringe2Phase.uniform("pitch1", 60);
  m_fringe2Phase.uniform("pitch2", 63);

  m_phaseFilter.init();
  m_phaseFilter.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  m_phaseFilter.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/HorizontalMedianFilter.frag"));
  m_phaseFilter.bindAttributeLocation("vert", 0);
  m_phaseFilter.bindAttributeLocation("vertTexCoord", 1);
  m_phaseFilter.link();
  m_phaseFilter.uniform("image", 0);
  //  In the shader these are floating point, so ensure that they are with a cast
  m_phaseFilter.uniform("width", ( float )width );
  m_phaseFilter.uniform("height", ( float )height );
  m_gaussFilter.init();
  m_gaussFilter.setImageDimensions( width, height );

  m_wrapped2Unwrapped.init();
  m_wrapped2Unwrapped.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  m_wrapped2Unwrapped.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/Wrapped2Unwrapped.frag"));
  m_wrapped2Unwrapped.bindAttributeLocation("vert", 0);
  m_wrapped2Unwrapped.bindAttributeLocation("vertTexCoord", 1);
  m_wrapped2Unwrapped.link();
  m_wrapped2Unwrapped.uniform("unfilteredPhase", 0);
  m_wrapped2Unwrapped.uniform("filteredPhase", 1);
  // TODO - Remove this hardcoding
  m_wrapped2Unwrapped.uniform("pitch1", 60.0f);
  m_wrapped2Unwrapped.uniform("pitch2", 63.0f);

  m_phase2Depth.init();
  m_phase2Depth.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  m_phase2Depth.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/Phase2Coordinate.frag"));
  m_phase2Depth.bindAttributeLocation("vert", 0);
  m_phase2Depth.bindAttributeLocation("vertTexCoord", 1);
  m_phase2Depth.link();
  m_phase2Depth.uniform("actualPhase", 0);
  // TODO - Remove this hardcoding
  // Camera Properties
  m_phase2Depth.uniform("cameraWidth", width);
  m_phase2Depth.uniform("cameraHeight", height);
  m_phase2Depth.uniform("cameraMatrix", m_cameraCalibration->GetIntrinsicAsMat() * m_cameraCalibration->GetExtrinsicAsMat());
  
  // Projector properties
  m_phase2Depth.uniform("fringePitch", 60);
  m_phase2Depth.uniform("Phi0", -5.1313f);
  m_phase2Depth.uniform("projectorMatrix", m_projectorCalibration->GetIntrinsicAsMat() * m_projectorCalibration->GetExtrinsicAsMat());

  m_phaseMap0.init		( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_phaseMap1.init		( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_phaseMap2.init		( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_depthMap.init		( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );

  m_imageProcessor.init( width, height );
  m_imageProcessor.setTextureAttachPoint( m_phaseMap0,		GL_COLOR_ATTACHMENT0 );
  m_imageProcessor.setTextureAttachPoint( m_phaseMap1,		GL_COLOR_ATTACHMENT1 );
  m_imageProcessor.setTextureAttachPoint( m_phaseMap2,		GL_COLOR_ATTACHMENT2 );
  m_imageProcessor.setTextureAttachPoint( m_depthMap,		GL_COLOR_ATTACHMENT3 );
  m_imageProcessor.unbind( );

  m_isInit = true;
  OGLStatus::logOGLErrors("SixFringeProcessor - Init( shared_ptr<IOpenGLReadBuffer> )");
}

const int SixFringeProcessor::GetWidth( void )
  { return m_depthMap.getWidth( ); }

const int SixFringeProcessor::GetHeight( void )
  { return m_depthMap.getHeight( ); }

void SixFringeProcessor::BindDepthMap( GLenum texture )
  { m_depthMap.bind(texture); }

void SixFringeProcessor::BindFringeImage( GLenum texture )
  { ( *m_inputBuffer->ReadBuffersBegin( ) )->ReadTexture( ).bind( texture ); }

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
	_wrapPhase( GL_COLOR_ATTACHMENT0, m_inputBuffer );
	_filterPhase( GL_COLOR_ATTACHMENT1, m_phaseMap0 );
	_gaussianFilter( GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT1, m_phaseMap1, m_phaseMap2);
	_unwrapPhase(GL_COLOR_ATTACHMENT2, m_phaseMap0, m_phaseMap1);
	_calculateDepth( GL_COLOR_ATTACHMENT3, m_phaseMap2 );
  }
  m_imageProcessor.unbind();
}

void SixFringeProcessor::_wrapPhase(GLenum drawBuffer, shared_ptr<MultiOpenGLBuffer> fringeBuffer)
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

void SixFringeProcessor::_gaussianFilter( GLenum pass1DrawBuffer, GLenum pass2DrawBuffer, Texture& pass1ReadBuffer, Texture& pass2ReadBuffer )
{
  // 1st dimension
  m_imageProcessor.bindDrawBuffer( pass1DrawBuffer );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  m_gaussFilter.bind( );
  pass1ReadBuffer.bind( GL_TEXTURE0 );
  m_imageProcessor.process( );

  // 2nd dimension
  m_imageProcessor.bindDrawBuffer( pass2DrawBuffer );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  m_gaussFilter.flipFilter( );
  pass2ReadBuffer.bind( GL_TEXTURE0 );
  m_imageProcessor.process( );
}

void SixFringeProcessor::_unwrapPhase( GLenum drawBuffer, Texture& unfilteredPhase, Texture& filteredPhase )
{
  m_imageProcessor.bindDrawBuffer( drawBuffer );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_wrapped2Unwrapped.bind( );
  unfilteredPhase.bind( GL_TEXTURE0 );
  filteredPhase.bind( GL_TEXTURE1 );
  m_imageProcessor.process( );
}

void SixFringeProcessor::_calculateDepth( GLenum drawBuffer, Texture& phase )
{
  m_imageProcessor.bindDrawBuffer( drawBuffer );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_phase2Depth.bind( );
  phase.bind( GL_TEXTURE0 );
  m_imageProcessor.process( );
}