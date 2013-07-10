#include <iostream>

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
#include "TriMesh.h"

class FringeProcessorShaderTest : public ::testing::Test
{
protected:
  const static int	  width = 256;
  const static int	  height = 256;
  QGLWidget			  glContext;
  wrench::gl::FBO	  shaderProcessor;
  
  wrench::gl::Texture inputTexture0Float;
  wrench::gl::Texture inputTexture1Float;
  wrench::gl::Texture outputTextureFloat;

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
	inputTexture1Float.init( width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT );
	outputTextureFloat.init( width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT );
	
	shaderProcessor.init( width, height );
	shaderProcessor.setTextureAttachPoint( outputTextureFloat, GL_COLOR_ATTACHMENT0 );
  }

  virtual void CheckImageValues( cv::Mat expectedOut, cv::Mat texture0, cv::Mat texture1 )
  {
	ASSERT_TRUE( inputTexture0Float.transferToTexture( texture0 ) );
	ASSERT_TRUE( inputTexture1Float.transferToTexture( texture1 ) );

	shaderProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	inputTexture0Float.bind( GL_TEXTURE0 );
	inputTexture1Float.bind( GL_TEXTURE1 );
	
	shaderProcessor.process( );
	IplImage* outputImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 4);
	ASSERT_TRUE(outputTextureFloat.transferFromTexture(outputImage));

	//TestUtils::WritePFM("Out.pfm", cv::Mat(outputImage)); // Uncomment if you need to see the output image

	double norm = cv::norm( cv::Mat(outputImage) - expectedOut );
	// Cant use EXPECT_FLOAT_EQ since there are rounding errors between different GPUs
	EXPECT_NEAR( 0.0, norm, .001f );
  }
};


TEST_F(FringeProcessorShaderTest, Coordinate2Holo)
{
  // Load our test data
  cv::Mat coordinateMap = TestUtils::LoadPFM("data/FlatCoordinateMap.pfm");
  cv::cvtColor( coordinateMap, coordinateMap, CV_BGRA2RGBA );
  cv::Mat coordinateHolo = TestUtils::LoadPFM("data/FlatCoordinateHolo.pfm");
  cv::cvtColor(coordinateHolo, coordinateHolo, CV_BGRA2RGBA);

  wrench::gl::ShaderProgram shader;
  shader.init( );
  shader.attachShader( new wrench::gl::Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert") );
  shader.attachShader( new wrench::gl::Shader(GL_FRAGMENT_SHADER, "Shaders/Coordinate2Holo.frag") );
  shader.bindAttributeLocation("vert", 0);
  shader.bindAttributeLocation("vertTexCoord", 1);
  shader.link( );
  
  shader.uniform( "coordinateMap", 0 );
  shader.uniform( "modelView", glm::mat4( ) ); // Just using the identity
  shader.uniform( "projectorModelView", glm::mat4( ) ); // Test data is set for identity
  shader.uniform( "projectionMatrix", glm::ortho( 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f ) );
  shader.uniform( "fringeFrequency", 2.0f);

  shaderProcessor.bind( );
  shader.bind( );

  // TODO - This test fails since the texture coordinates range [.002, .998] instead of [0, 1]
  CheckImageValues( coordinateHolo, coordinateMap, coordinateMap );
}

TEST_F(FringeProcessorShaderTest, Wrapped2Unwrapped)
{
  cv::Mat wrappedPhase = TestUtils::LoadPFM( "data/WrappedPhaseComponents-30-39.pfm" );
  cv::cvtColor( wrappedPhase, wrappedPhase, CV_BGRA2RGBA );

  cv::Mat unwrappedPhase = TestUtils::LoadPFM( "data/UnwrappedPhase-30-39.pfm" );
  cv::cvtColor( unwrappedPhase, unwrappedPhase, CV_BGRA2RGBA );

  wrench::gl::ShaderProgram shader;
  shader.init();
  shader.attachShader(new wrench::gl::Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  shader.attachShader(new wrench::gl::Shader(GL_FRAGMENT_SHADER, "Shaders/Wrapped2Unwrapped.frag"));
  shader.bindAttributeLocation("vert", 0);
  shader.bindAttributeLocation("vertTexCoord", 1);
  shader.link();
  shader.uniform("unfilteredPhase", 0);
  shader.uniform("filteredPhase", 1);
  shader.uniform("pitch1", 30);
  shader.uniform("pitch2", 39);
  shader.uniform( "m", .0483f * 256.0f );
  shader.uniform( "b1", -1.0f / 256.0f );
  shader.uniform( "b2", -5.0f / 256.0f );
  shader.uniform( "b3", -6.0f / 256.0f );
  shader.uniform( "rightSide", false);
  
  // Now do the actual shader pass to see if we get the expected output
  shaderProcessor.bind( );
  shader.bind( );

  CheckImageValues( unwrappedPhase, wrappedPhase, wrappedPhase );
}

TEST_F(FringeProcessorShaderTest, Fringe2WrappedPhase)
{
  cv::Mat fringe1 = TestUtils::LoadPFM( "data/fringe1-30.pfm" );
  cv::cvtColor( fringe1, fringe1, CV_BGRA2RGBA );
  cv::Mat fringe2 = TestUtils::LoadPFM( "data/fringe2-39.pfm" );
  cv::cvtColor( fringe2, fringe2, CV_BGRA2RGBA );

  cv::Mat wrappedPhase = TestUtils::LoadPFM( "data/WrappedPhaseComponents-30-39.pfm" );
  cv::cvtColor( wrappedPhase, wrappedPhase, CV_BGRA2RGBA );

  wrench::gl::ShaderProgram shader;
  shader.init();
  shader.attachShader(new wrench::gl::Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  shader.attachShader(new wrench::gl::Shader(GL_FRAGMENT_SHADER, "Shaders/Fringe2WrappedPhase.frag"));
  shader.bindAttributeLocation("vert", 0);
  shader.bindAttributeLocation("vertTexCoord", 1);
  shader.link();
  shader.uniform("fringeImage1", 0);
  shader.uniform("fringeImage2", 1);
  shader.uniform("pitch1", 30);
  shader.uniform("pitch2", 39);

  shaderProcessor.bind( );
  shader.bind( );

  // Now check some values!
  CheckImageValues( wrappedPhase, fringe1, fringe2 );
}

TEST_F(FringeProcessorShaderTest, Phase2Coordinate)
{
  wrench::gl::ShaderProgram shader;
  shader.init();
  shader.attachShader(new wrench::gl::Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  shader.attachShader(new wrench::gl::Shader(GL_FRAGMENT_SHADER, "Shaders/Phase2Coordinate.frag"));
  shader.bindAttributeLocation("vert", 0);
  shader.bindAttributeLocation("vertTexCoord", 1);
  shader.link();
  shader.uniform("actualPhase", 0);
  
  // Camera properties
  glm::mat4x3 cameraMatrix(2053.340885098, -96.319507896, -0.423394, 3.9470993885, -2465.782916025, -0.0173105, -1415.44646598, -232.26932152, -0.90578, 275718.130302, 427396.406563, 1090.39);
  shader.uniform("cameraWidth", 800);
  shader.uniform("cameraHeight", 600);
  shader.uniform("cameraMatrix", cameraMatrix);

  // Projector properties
  glm::mat4x3 projectorMatrix(-4789.23545076463, -144.35441948414, -0.00185357, -119.1250566882, 4769.1063090524, 0.0714162, -361.81377941068, 60.912146885, -0.997445, 1660295.92904, 888222.6923, 1806.64);
  shader.uniform("Phi0", -5.1313f);
  shader.uniform("fringePitch", 60);
  shader.uniform("projectorMatrix", projectorMatrix);

  // Now do the actual shader pass to see if we get the expected output
  shaderProcessor.bind( );
  shader.bind( );

  // Now check some values! // TODO - Comeback and fix this
  //CheckValue( cv::Scalar(281.01715f, 105.57656f, 602.74893f, 1.0f), cv::Scalar(2.0f) );
  //CheckValue( cv::Scalar(274.27292f, 106.76128f, 592.96862f, 1.0f), cv::Scalar(5.0f) );
}