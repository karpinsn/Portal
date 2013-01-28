#include "SixFringeProcessor.h"

SixFringeProcessor::SixFringeProcessor( void ) :
  m_isInit(false), m_captureReference(true), m_shift(0.0f), m_scale(1.0)
{ }

void SixFringeProcessor::Init( shared_ptr<IOpenGLReadBuffer> inputBuffer, shared_ptr<IOpenGLWriteBuffer> outputBuffer )
{
  m_inputBuffer	  = inputBuffer;
  m_outputBuffer  = outputBuffer;
  m_outputBuffer->InitWrite( m_inputBuffer->GetWidth( ), m_inputBuffer->GetHeight( ) );

  //  Make sure we are the current OpenGL Context
  makeCurrent( );

  //  Initialize our shaders
  m_fringe2Phase.init();
  m_fringe2Phase.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/Fringe2Phase.vert"));
  m_fringe2Phase.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/Fringe2Phase.frag"));
  m_fringe2Phase.bindAttributeLocation("vert", 0);
  m_fringe2Phase.bindAttributeLocation("vertTexCoord", 1);
  m_fringe2Phase.link();
  m_fringe2Phase.uniform("fringeImage1", 0);
  m_fringe2Phase.uniform("fringeImage2", 1); 
  m_fringe2Phase.uniform("gammaCutoff", 0.0f);
  m_fringe2Phase.uniform("pitch1", 74.0f);
  m_fringe2Phase.uniform("pitch2", 79.0f);

  m_phaseFilter.init();
  m_phaseFilter.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/PhaseFilter.vert"));
  m_phaseFilter.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/PhaseFilter.frag"));
  m_phaseFilter.bindAttributeLocation("vert", 0);
  m_phaseFilter.bindAttributeLocation("vertTexCoord", 1);
  m_phaseFilter.link();
  m_phaseFilter.uniform("image", 0);
  //  In the shader these are floating point, so ensure that they are with a cast
  m_phaseFilter.uniform("width", ( float )inputBuffer->GetWidth( ) );
  m_phaseFilter.uniform("height", ( float )inputBuffer->GetHeight( ) );
	
  m_phase2Depth.init();
  m_phase2Depth.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/Phase2Depth.vert"));
  m_phase2Depth.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/Phase2Depth.frag"));
  m_phase2Depth.bindAttributeLocation("vert", 0);
  m_phase2Depth.bindAttributeLocation("vertTexCoord", 1);
  m_phase2Depth.link();
  m_phase2Depth.uniform("actualPhase", 0);
  m_phase2Depth.uniform("referencePhase", 1);
  m_phase2Depth.uniform("scale", m_scale);
  m_phase2Depth.uniform("shift", m_shift);
	
  m_depth2Holo.init();
  m_depth2Holo.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/Depth2Holo.vert"));
  m_depth2Holo.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/Depth2Holo.frag"));
  m_depth2Holo.link();
  m_depth2Holo.uniform("fringeFrequency", 16.0f);
  m_depth2Holo.uniform("depthMap", 0);

  m_phaseMap0.init		( inputBuffer->GetWidth( ), inputBuffer->GetHeight( ), GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_phaseMap1.init		( inputBuffer->GetWidth( ), inputBuffer->GetHeight( ), GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_referencePhase.init	( inputBuffer->GetWidth( ), inputBuffer->GetHeight( ), GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_depthMap.init		( inputBuffer->GetWidth( ), inputBuffer->GetHeight( ), GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
	
  m_imageProcessor.init(inputBuffer->GetWidth( ), inputBuffer->GetHeight( ) );
  m_imageProcessor.setTextureAttachPoint( m_phaseMap0,					  GL_COLOR_ATTACHMENT0 );
  m_imageProcessor.setTextureAttachPoint( m_phaseMap1,					  GL_COLOR_ATTACHMENT1 );
  m_imageProcessor.setTextureAttachPoint( m_referencePhase,				  GL_COLOR_ATTACHMENT2 );
  m_imageProcessor.setTextureAttachPoint( m_depthMap,					  GL_COLOR_ATTACHMENT3 );
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

void SixFringeProcessor::paintGL( void )
{
  // If we are not init then just return
  if( !m_isInit )
	{ return; }

  OGLStatus::logOGLErrors("SixFringeProcessor - paintGL( )");

  //  Make sure we are the current OpenGL Context
  makeCurrent( );

  // Our actual decoding is done here
  m_imageProcessor.bind();
  {
	_calculatePhase( GL_COLOR_ATTACHMENT0 );
	_filterPhase( GL_COLOR_ATTACHMENT1, m_phaseMap0 );

	if( m_captureReference )
	{ 
	  _calculatePhase( GL_COLOR_ATTACHMENT2 );
	  _filterPhase( GL_COLOR_ATTACHMENT2, m_phaseMap1 ); 

	  m_captureReference = false;
	}
	else
	{ 
	  _calculateDepth( GL_COLOR_ATTACHMENT3, m_phaseMap0 );

	  m_imageProcessor.setTextureAttachPoint( m_outputBuffer->WriteBuffer( ), GL_COLOR_ATTACHMENT4 );
	  _holoEncode( GL_COLOR_ATTACHMENT4 );
	}
  }
  m_imageProcessor.unbind();

  m_outputBuffer->WriteFinished( );
}

void SixFringeProcessor::_calculatePhase(GLenum drawBuffer)
{
  m_imageProcessor.bindDrawBuffer( drawBuffer );
  m_fringe2Phase.bind( );
  m_inputBuffer->BindBuffer( GL_TEXTURE0 );
  m_imageProcessor.process( );
}

void SixFringeProcessor::_filterPhase( GLenum drawBuffer, Texture& phase2Filter )
{
  m_imageProcessor.bindDrawBuffer( drawBuffer );
  m_phaseFilter.bind( );
  phase2Filter.bind( GL_TEXTURE0 );
  m_imageProcessor.process( );
}

void SixFringeProcessor::_calculateDepth( GLenum drawBuffer, Texture& phase )
{
  m_imageProcessor.bindDrawBuffer( drawBuffer );
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
  m_depth2Holo.bind();
  m_depthMap.bind( GL_TEXTURE0 );
  m_imageProcessor.process( );
}