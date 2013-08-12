#ifdef __APPLE__
#include <glew.h>
#include <OpenGL/gl.h>
#elif _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#endif

#include <iostream>
#include <memory>

#include <QApplication>
#include <QGLWidget>
#include <QString>

#include <wrench/gl/Texture.h>

#include "gtest/gtest.h"

#include "TestUtils.h"
#include "PortalProcessor.h"

class MockProcessContext : public IProcessContext
{
private:
  Texture m_coordMap;

public:
  MockProcessContext( cv::Mat coordMap )
  {
	m_coordMap.init	( coordMap.cols, coordMap.rows, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
	m_coordMap.transferToTexture( coordMap );
  }

  void       Init( void ) { } // Dont need to do anything

  const int  GetWidth( void )
	{ return m_coordMap.getWidth( ); }
  
  const int  GetHeight( void )
	{ return m_coordMap.getHeight( ); }

  void       BindCoordMap( GLenum texture )
	{ m_coordMap.bind( texture ); }

  void       BindFringeImage( GLenum texture )
	{ throw "Unimplemented!"; }

  void       BindTextureMap( GLenum texture )
	{ throw "Unimplemented!"; }

  void       Process( void ) { } // Dont need to do anything 
};

class PortalProcessorTest : public ::testing::Test
{
protected:
  unique_ptr<PortalProcessor>    processor;
  unique_ptr<ITripleBuffer>		 outputBuffer;
  static const int               width = 512;
  static const int               height = 512;

  virtual void SetUp( )
  {
	// This should create an OpenGL context for us
	processor = unique_ptr<PortalProcessor>( new PortalProcessor( ) );
	processor->makeCurrent( );
	processor->updateGL( );

	// Now need to init GLEW so we can do fancy OpenGL
	ASSERT_EQ( GLEW_OK, glewInit( ) );

	processor->SetProperty<int>( "outputWidth", width );
	processor->SetProperty<int>( "outputHeight", height );
	processor->SetProperty<float>( "pointSize", 3.0f );
	processor->SetProperty<float>( "fringeFrequency", 8.0f );

	outputBuffer = unique_ptr<OpenGLTripleBuffer>( new OpenGLTripleBuffer( nullptr, false, false ) );
  }

  void RunProcess( void )
  {
	processor->paintGL( );
  }

  Texture& GetHoloFrame( )
	{ return processor->m_encodedMap; }
};

TEST_F( PortalProcessorTest, CheckDualSystem )
{
  auto leftCoordMap = TestUtils::LoadPFM( "data/capture2/LeftCoords.pfm" );
  cv::cvtColor( leftCoordMap, leftCoordMap, CV_BGRA2RGBA );
  auto leftProcessor = unique_ptr<MockProcessContext>( new MockProcessContext( leftCoordMap ) );
  processor->AddProcessContext( leftProcessor.get( ) );

  auto rightCoordMap = TestUtils::LoadPFM( "data/capture2/RightCoords.pfm" );
  cv::cvtColor( rightCoordMap, rightCoordMap, CV_BGRA2RGBA );
  auto rightProcessor = unique_ptr<MockProcessContext>( new MockProcessContext( rightCoordMap ) );
  processor->AddProcessContext( rightProcessor.get( ) );

  processor->Init( outputBuffer.get( ) );
  RunProcess( );

  // At this point the Holovideo compressed frame should be in the working buffer of the outputBuffer
  //outputBuffer->StartRead( );
  //auto holoFrame = outputBuffer->ReadTexture( );
  IplImage* outputImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 4);
  GetHoloFrame( ).transferFromTexture( outputImage );

  TestUtils::WritePFM( "Output.pfm", cv::Mat( outputImage ) );
}