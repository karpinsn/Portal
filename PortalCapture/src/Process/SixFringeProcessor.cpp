#include "SixFringeProcessor.h"

SixFringeProcessor::SixFringeProcessor( void ) :
  m_isInit(false), m_captureReference(true)
{ }

void SixFringeProcessor::Init( shared_ptr<IOpenGLReadBuffer> inputBuffer, shared_ptr<IOpenGLWriteBuffer> outputBuffer )
{
  m_inputBuffer	  = inputBuffer;
  m_outputBuffer  = outputBuffer;
  m_outputBuffer->InitWrite( m_inputBuffer->GetWidth( ), m_inputBuffer->GetHeight( ) );

  //  Make sure we are the current OpenGL Context
  makeCurrent( );

  //  Initialize our shaders
  m_phaseCalculator.init();
  m_phaseCalculator.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/PhaseCalculator.vert"));
  m_phaseCalculator.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/PhaseCalculator.frag"));
  m_phaseCalculator.bindAttributeLocation("vert", 0);
  m_phaseCalculator.bindAttributeLocation("vertTexCoord", 1);
  m_phaseCalculator.link();
  m_phaseCalculator.uniform("fringeImage1", 0);
  m_phaseCalculator.uniform("fringeImage2", 1); 
  m_phaseCalculator.uniform("gammaCutoff", 0.0f);

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
	
  m_depthCalculator.init();
  m_depthCalculator.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/DepthCalculator.vert"));
  m_depthCalculator.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/DepthCalculator.frag"));
  m_depthCalculator.bindAttributeLocation("vert", 0);
  m_depthCalculator.bindAttributeLocation("vertTexCoord", 1);
  m_depthCalculator.link();
  m_depthCalculator.uniform("actualPhase", 0);
  m_depthCalculator.uniform("referencePhase", 1);
  //  TODO: Scaling factor
	
  m_phaseMap0.init		( inputBuffer->GetWidth( ), inputBuffer->GetHeight( ), GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_phaseMap1.init		( inputBuffer->GetWidth( ), inputBuffer->GetHeight( ), GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
  m_referencePhase.init	( inputBuffer->GetWidth( ), inputBuffer->GetHeight( ), GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
	
  m_imageProcessor.init(inputBuffer->GetWidth( ), inputBuffer->GetHeight( ) );
  m_imageProcessor.setTextureAttachPoint( m_phaseMap0,					  GL_COLOR_ATTACHMENT0 );
  m_imageProcessor.setTextureAttachPoint( m_phaseMap1,					  GL_COLOR_ATTACHMENT1 );
  //m_imageProcessor.setTextureAttachPoint( m_outputBuffer->WriteBuffer( ),  GL_COLOR_ATTACHMENT2 );
  m_imageProcessor.setTextureAttachPoint( m_referencePhase,				  GL_COLOR_ATTACHMENT3 );
  OGLStatus::logFBOStatus( );
  m_imageProcessor.unbind( );

  m_isInit = true;
  OGLStatus::logOGLErrors("SixFringeProcessor - Init( shared_ptr<IOpenGLReadBuffer> )");
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
	  _filterPhase( GL_COLOR_ATTACHMENT3, m_phaseMap1 ); 
	  m_captureReference = false;
	}
	else
	{ 
	  m_imageProcessor.setTextureAttachPoint( m_outputBuffer->WriteBuffer( ), GL_COLOR_ATTACHMENT2 );
	  _calculateDepth( GL_COLOR_ATTACHMENT2, m_phaseMap1 ); 
	}
  }
  m_imageProcessor.unbind();

  m_outputBuffer->WriteFinished( );
}

void SixFringeProcessor::_calculatePhase(GLenum drawBuffer)
{
  m_imageProcessor.bindDrawBuffer( drawBuffer );
  m_phaseCalculator.bind( );
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
  m_depthCalculator.bind( );
  phase.bind( GL_TEXTURE0 );
  m_referencePhase.bind( GL_TEXTURE1 );
  m_imageProcessor.process( );
}