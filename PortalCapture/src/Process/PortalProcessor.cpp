#include "PortalProcessor.h"

PortalProcessor::PortalProcessor( void ) :
  m_isInit(false), m_captureReference(true), m_outputTexture(&m_encodedMap)
{ }

void PortalProcessor::AddProcessContext( shared_ptr<IProcessContext> processContext )
{
  m_captureProcessors.push_back( processContext );
}

void PortalProcessor::Init( shared_ptr<IWriteBuffer> outputBuffer )
{
  Utils::AssertOrThrowIfFalse(nullptr != outputBuffer, "Invalid output buffer");
  Utils::AssertOrThrowIfFalse(0 < m_captureProcessors.size(), "Must have an input capture buffer");

  //  All the buffers should be the same size. Just grab the first one
  //int width = m_captureProcessors[0]->GetWidth();
  //int height = m_captureProcessors[0]->GetHeight();
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
	(*itr)->Init( );
  }

  //  Initialize our shaders	
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

  m_encodedMap.init		( width, height, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );

  m_imageProcessor.init( width, height );
  m_imageProcessor.setTextureAttachPoint( m_encodedMap,	GL_COLOR_ATTACHMENT0 );
  m_imageProcessor.unbind( );

  m_isInit = true;
  OGLStatus::logOGLErrors("PortalProcessor - Init( shared_ptr<IOpenGLReadBuffer> )");
}

void PortalProcessor::CaptureReference( void )
{
  wrench::Logger::logDebug("Capturing a reference plane");
  m_captureReference = true;
}

void PortalProcessor::paintGL( void )
{
  // If we are not init then just return
  if( !m_isInit )
	{ return; }

  OGLStatus::logOGLErrors("PortalProcessor - paintGL( )");

  //  Make sure we are the current OpenGL Context
  makeCurrent( );

  if (m_captureReference)
  {
	_CaptureReference( );
  }
  else
  {
	_Process( );
  }
}

void PortalProcessor::_CaptureReference( void )
{
  OGLStatus::logOGLErrors("PortalProcessor - _CaptureReference( )");

  // Process input on each capture processor
  for ( auto itr = m_captureProcessors.begin( ); itr != m_captureProcessors.end( ); ++itr )
  {
	(*itr)->CaptureReference( );
  }

  m_captureReference = false;
}

void PortalProcessor::_Process( void )
{
  OGLStatus::logOGLErrors("PortalProcessor - _ProcessInput( )");
  for ( auto itr = m_captureProcessors.begin( ); itr != m_captureProcessors.end( ); ++itr )
  {
	(*itr)->Process( );
  }

  // Now that we have processed, we need to rectify everything into one scan
  m_imageProcessor.bind();
  {
	//	Rectify and Encode
	m_imageProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT0 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_depth2Holo.bind();
	
	for ( int processor = 0; processor < m_captureProcessors.size(); ++processor )
	  { m_captureProcessors[processor]->BindDepthMap( GL_TEXTURE0 + processor ); }

	m_imageProcessor.process( );

	// Output results
	m_imageProcessor.setTextureAttachPoint( m_outputBuffer->StartWriteTexture( ), GL_COLOR_ATTACHMENT1 );
	m_imageProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT1 );
	m_renderTexture.bind( );
	m_encodedMap.bind( GL_TEXTURE0 );
	m_imageProcessor.process( );
  }
  m_imageProcessor.unbind();

  m_outputBuffer->WriteFinished( );
}