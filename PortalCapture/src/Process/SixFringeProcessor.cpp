#include "SixFringeProcessor.h"

SixFringeProcessor::SixFringeProcessor( void ) :
  m_isInit(false)
{ }

void SixFringeProcessor::Init( shared_ptr<IOpenGLReadBuffer> inputBuffer )
{
  m_inputBuffer = inputBuffer;

  //  Initialize our shaders
  m_phaseCalculator.init();
  m_phaseCalculator.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/SixFringe/PhaseCalculator.vert"));
  m_phaseCalculator.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/SixFringe/PhaseCalculator.frag"));
  m_phaseCalculator.bindAttributeLocation("vert", 0);
  m_phaseCalculator.bindAttributeLocation("vertTexCoord", 1);
  m_phaseCalculator.link();
  m_phaseCalculator.uniform("fringeImage1", 0);
  m_phaseCalculator.uniform("fringeImage2", 1); 
  //  TODO: GammaCutoff

  m_phaseFilter.init();
  m_phaseFilter.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/SixFringe/PhaseFilter.vert"));
  m_phaseFilter.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/SixFringe/PhaseFilter.frag"));
  m_phaseFilter.bindAttributeLocation("vert", 0);
  m_phaseFilter.bindAttributeLocation("vertTexCoord", 1);

  m_phaseFilter.link();
  m_phaseFilter.uniform("image", 0);
  m_phaseFilter.uniform("width", inputBuffer->GetWidth( ));
  m_phaseFilter.uniform("height", inputBuffer->GetHeight( ));

   m_depthCalculator.init();
  m_depthCalculator.attachShader(new Shader(GL_VERTEX_SHADER, "Shaders/SixFringe/DepthCalculator.vert"));
  m_depthCalculator.attachShader(new Shader(GL_FRAGMENT_SHADER, "Shaders/SixFringe/DepthCalculator.frag"));
  m_depthCalculator.bindAttributeLocation("vert", 0);
  m_depthCalculator.bindAttributeLocation("vertTexCoord", 1);
  m_depthCalculator.link();
  m_depthCalculator.uniform("actualPhase", 0);
  m_depthCalculator.uniform("referencePhase", 1);
  //  TODO: Scaling factor

  m_phaseMap0.init(inputBuffer->GetWidth( ), inputBuffer->GetHeight( ), GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT);
  m_phaseMap1.init(inputBuffer->GetWidth( ), inputBuffer->GetHeight( ), GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT);
  m_depthMap.init (inputBuffer->GetWidth( ), inputBuffer->GetHeight( ), GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT);

  m_imageProcessor.init(inputBuffer->GetWidth( ), inputBuffer->GetHeight( ));
  m_imageProcessor.setTextureAttachPoint(m_phaseMap0, GL_COLOR_ATTACHMENT0_EXT);
  m_imageProcessor.setTextureAttachPoint(m_phaseMap1, GL_COLOR_ATTACHMENT1_EXT);
  m_imageProcessor.setTextureAttachPoint(m_depthMap, GL_COLOR_ATTACHMENT2_EXT);
  m_imageProcessor.unbind();

  m_isInit = true;
}

void SixFringeProcessor::paintGL( void )
{
  //  If we are not init then just return
  if( !m_isInit )
  { return; }

  //  Our actual decoding is done here
  m_imageProcessor.bind();
  {
  }
  m_imageProcessor.unbind();
}