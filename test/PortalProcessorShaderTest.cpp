#include "gtest/gtest.h"

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

#include <memory>

#include <QApplication>
#include <QGLWidget>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <wrench/gl/OGLStatus.h>
#include <wrench/gl/ShaderProgram.h>
#include <Wrench/gl/GaussProgram.h>
#include <wrench/gl/Shader.h>
#include <wrench/gl/Texture.h>
#include <wrench/gl/FBO.h>

#include "TestUtils.h"
#include "SplatField.h"

class PortalProcessorShaderTest : public ::testing::Test
{
protected:
  const static int	  width = 256;
  const static int	  height = 256;
  QGLWidget			  glContext;
  wrench::gl::FBO	  shaderProcessor;
  
  wrench::gl::Texture inputTexture0Float;
  wrench::gl::Texture inputTexture1char;
  wrench::gl::Texture outputTextureFloat;
  wrench::gl::Texture outputTextureDepth;
  unique_ptr<SplatField> mesh;

  virtual void SetUp()
  {
	// This should create an OpenGL context for us
	ASSERT_TRUE( glContext.isValid( ) );
	glContext.makeCurrent( );
	glContext.updateGL( );

	// Now need to init GLEW so we can do fancy OpenGL
	ASSERT_EQ( GLEW_OK, glewInit( ) );

	// Init our state
	glEnable( GL_PROGRAM_POINT_SIZE );
	glEnable( GL_DEPTH_TEST );

	inputTexture0Float.init( width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT );
	inputTexture1char.init( 64, 64, GL_RGBA, GL_RGB, GL_UNSIGNED_BYTE );
	outputTextureFloat.init( width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT );
	outputTextureDepth.init( width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT );
	
	shaderProcessor.init( width, height );
	shaderProcessor.setTextureAttachPoint( outputTextureFloat, GL_COLOR_ATTACHMENT0 );
	shaderProcessor.setTextureAttachPoint( outputTextureDepth, GL_DEPTH_ATTACHMENT );
	shaderProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT0 );  

	mesh = unique_ptr<SplatField>( new SplatField( width, height ) );
  }

  virtual void Render( cv::Mat texture0, cv::Mat texture1 )
  {
	if( !texture0.empty( ) )
	  { ASSERT_TRUE( inputTexture0Float.transferToTexture( texture0 ) ); }
	if( !texture1.empty( ) )
	  { ASSERT_TRUE( inputTexture1char.transferToTexture( texture1 ) ); }

	glPushAttrib(GL_VIEWPORT_BIT);
	{
	  glViewport (0, 0, width, height);
	  inputTexture0Float.bind( GL_TEXTURE0 );
	  inputTexture1char.bind( GL_TEXTURE1 );
	  mesh->draw( );
	}
	glPopAttrib();
  }
};

TEST_F(PortalProcessorShaderTest, CoordinateRectifierDepth)
{
  cv::Mat leftCam = TestUtils::LoadPFM( "data/LeftCamCoordinates.pfm" );
  cv::cvtColor( leftCam, leftCam, CV_BGRA2RGBA );
  cv::Mat rightCam = TestUtils::LoadPFM( "data/RightCamCoordinates.pfm" );
  cv::cvtColor( rightCam, rightCam, CV_BGRA2RGBA );
  cv::Mat mergedDepth = TestUtils::LoadPFM( "data/MergedDepth.pfm" );

  wrench::gl::ShaderProgram shader;
  shader.init();
  shader.attachShader(new wrench::gl::Shader(GL_VERTEX_SHADER, "Shaders/CoordinateRectifier.vert"));
  shader.attachShader(new wrench::gl::Shader(GL_FRAGMENT_SHADER, "Shaders/CoordinateRectifierDepth.frag"));
  shader.bindAttributeLocation("vert", 0);
  shader.bindAttributeLocation("vertTexCoord", 1);
  shader.link();

  shader.uniform( "coordinateMap", 0 );
  shader.uniform( "modelView", glm::mat4( ) ); // Test data is set for identity
  shader.uniform( "projectionMatrix", glm::ortho( -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f ) );
  shader.uniform( "pointSize", 1.0f );
  shader.uniform( "delta", 0.0f );

  shaderProcessor.bind( );
  shader.bind( );

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Make sure we clear to transparent
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  Render( leftCam, cv::Mat( ) );
  Render( rightCam, cv::Mat( ) );  

  // Fetch the results and check
  IplImage* depthImage = cvCreateImage( cvSize(width, height), IPL_DEPTH_32F, 1 );
  ASSERT_TRUE( outputTextureDepth.transferFromTexture( depthImage ) );
  
  //TestUtils::WritePFM( "Out.pfm", cv::Mat( depthImage ) ); // Uncomment if you need to see the output image

  double norm = cv::norm( cv::Mat(depthImage) - mergedDepth );
  // Cant use EXPECT_FLOAT_EQ since there are rounding errors between different GPUs
  EXPECT_NEAR( 0.0, norm, .001f );
}

TEST_F(PortalProcessorShaderTest, CoordinateRectifierSplat)
{
  cv::Mat leftCam = TestUtils::LoadPFM( "data/LeftCamCoordinates.pfm" );
  cv::cvtColor( leftCam, leftCam, CV_BGRA2RGBA );
  cv::Mat rightCam = TestUtils::LoadPFM( "data/RightCamCoordinates.pfm" );
  cv::cvtColor( rightCam, rightCam, CV_BGRA2RGBA );
  auto blendImage = cv::imread("blend.png");

  wrench::gl::ShaderProgram shader;
  shader.init();
  shader.attachShader(new wrench::gl::Shader(GL_VERTEX_SHADER, "Shaders/CoordinateRectifier.vert"));
  shader.attachShader(new wrench::gl::Shader(GL_FRAGMENT_SHADER, "Shaders/CoordinateRectifierSplat.frag"));
  shader.bindAttributeLocation("vert", 0);
  shader.bindAttributeLocation("vertTexCoord", 1);
  shader.link();

  shader.uniform( "coordinateMap", 0 );
  shader.uniform( "blendMap", 1 );
  shader.uniform( "modelView", glm::mat4( ) ); // Test data is set for identity
  shader.uniform( "projectionMatrix", glm::ortho( -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f ) );
  shader.uniform( "pointSize", 1.5f );

  shaderProcessor.bind( );
  shader.bind( );

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Make sure we clear to transparent
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glDisable( GL_DEPTH_TEST ); // Disable depth test for this pass, we will manually do it
  Render( leftCam, blendImage );
  Render( rightCam, blendImage );  
  glEnable( GL_DEPTH_TEST ); // Now put it back

  // Fetch the results and check
  IplImage* blendedCoords = cvCreateImage( cvSize(width, height), IPL_DEPTH_32F, 4 );
  ASSERT_TRUE( outputTextureFloat.transferFromTexture( blendedCoords ) );
  
  cv::Mat blendedCoordMat( blendedCoords );
  // Quick normalize based on alpha. Not the most efficient but it will work
  for(int r = 0; r < height; ++r)
  {
	float* pixelPointer = blendedCoordMat.ptr<float>(r);
	for(int c = 0; c < width; ++c)
	{
	  // Skip 0.0 so we dont get NaNs
	  if( 0.0f == pixelPointer[c * 4 + 3] )
		{ continue; }
	  pixelPointer[c * 4] = pixelPointer[c * 4] / pixelPointer[c * 4 + 3];
	  pixelPointer[c * 4 + 1] = pixelPointer[c * 4 + 1] / pixelPointer[c * 4 + 3];
	  pixelPointer[c * 4 + 2] = pixelPointer[c * 4 + 2] / pixelPointer[c * 4 + 3];
	  pixelPointer[c * 4 + 3] = pixelPointer[c * 4 + 3] / pixelPointer[c * 4 + 3];
	}
  }

  //TestUtils::WritePFM( "Out.pfm", blendedCoordMat ); // Uncomment if you need to see the output image

  cv::Mat mergedDepth = TestUtils::LoadPFM("data/MergedCoordinates.pfm");
  cv::cvtColor( mergedDepth, mergedDepth, CV_BGRA2RGBA );
  double norm = cv::norm( // We are grabbing a ROI since there are some rounding errors around the edge of the sphere
	cv::Mat( blendedCoordMat, cv::Rect(80, 80, width - 160, height - 160) ) - 
	cv::Mat(mergedDepth, cv::Rect(80, 80, width - 160, height - 160) ) );
  
  // Cant use EXPECT_FLOAT_EQ since there are rounding errors between different GPUs
  EXPECT_NEAR( 0.0, norm, .001f ); 
}