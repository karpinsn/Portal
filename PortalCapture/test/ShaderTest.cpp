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

#include <wrench/gl/OGLStatus.h>
#include <wrench/gl/ShaderProgram.h>
#include <Wrench/gl/GaussProgram.h>
#include <wrench/gl/Shader.h>
#include <wrench/gl/Texture.h>
#include <wrench/gl/FBO.h>

class ShadersTest : public ::testing::Test
{
protected:
  const static int width = 64;
  const static int height = 64;
  QGLWidget			  glContext;
  wrench::gl::FBO	  shaderProcessor;
  
  wrench::gl::Texture inputTexture0Float;
  wrench::gl::Texture inputTexture1Float;
  wrench::gl::Texture outputTextureFloat;

  virtual void SetUp()
  {
	// This should create an OpenGL context for us
	EXPECT_TRUE( glContext.isValid( ) );
	glContext.makeCurrent( );
	glContext.updateGL( );

	// Now need to init GLEW so we can do fancy OpenGL
	EXPECT_EQ( GLEW_OK, glewInit( ) );

	inputTexture0Float.init( width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT );
	inputTexture1Float.init( width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT );
	outputTextureFloat.init( width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT );
	
	shaderProcessor.init( width, height );
	shaderProcessor.setTextureAttachPoint( outputTextureFloat, GL_COLOR_ATTACHMENT0 );
  }
};

TEST_F(ShadersTest, Wrapped2Unwrapped)
{
  wrench::gl::ShaderProgram shader;
  shader.init();
  shader.attachShader(new wrench::gl::Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  shader.attachShader(new wrench::gl::Shader(GL_FRAGMENT_SHADER, "Shaders/Wrapped2Unwrapped.frag"));
  shader.bindAttributeLocation("vert", 0);
  shader.bindAttributeLocation("vertTexCoord", 1);
  shader.link();
  shader.uniform("unfilteredPhase", 0);
  shader.uniform("filteredPhase", 1);
  shader.uniform("pitch1", 60.0f);
  shader.uniform("pitch2", 63.0f);
  
  // Now do the actual shader pass to see if we get the expected output
  shaderProcessor.bind( );
  shader.bind( );

  auto checkValue = [this] (cv::Scalar unfiltered, cv::Scalar filtered, float expected) {
	 // Transfer to the texture, then bind
	EXPECT_TRUE( inputTexture0Float.transferToTexture(cv::Mat(width, height, CV_32FC4, unfiltered)) );
	EXPECT_TRUE( inputTexture1Float.transferToTexture(cv::Mat(width, height, CV_32FC4, filtered)) );

	shaderProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	inputTexture0Float.bind( GL_TEXTURE0 );
	inputTexture1Float.bind( GL_TEXTURE1 );

	// Process and check our output
	shaderProcessor.process( );
	IplImage* outputImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 4);
	EXPECT_TRUE(outputTextureFloat.transferFromTexture(outputImage));
	EXPECT_FLOAT_EQ(expected, cv::Mat(outputImage).at<float>(0,0));
  };

  // Now check some values!
  checkValue(cv::Scalar(.4f, 0.0, 0.0, 0.0), cv::Scalar(0.0f, 0.0, 5.6832, 0.0), 113.497335529f);
  checkValue(cv::Scalar(2.0f, 0.0, 0.0, 0.0), cv::Scalar(0.0f, 0.0, .2, 0.0), 2.0f);
  checkValue(cv::Scalar(.8f, 0.0, 0.0, 0.0), cv::Scalar(0.0f, 0.0, 4.7832, 0.0), 95.0477796076f); 
}

TEST_F(ShadersTest, Phase2Depth)
{
  wrench::gl::ShaderProgram shader;
  shader.init();
  shader.attachShader(new wrench::gl::Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  shader.attachShader(new wrench::gl::Shader(GL_FRAGMENT_SHADER, "Shaders/Phase2Depth.frag"));
  shader.bindAttributeLocation("vert", 0);
  shader.bindAttributeLocation("vertTexCoord", 1);
  shader.link();
  shader.uniform("actualPhase", 0);
  shader.uniform("referencePhase", 1);
  shader.uniform("scale", 3.0f);
  shader.uniform("shift", 2.0f);
  shaderProcessor.bind( );
  shader.bind( );

  auto checkValue = [this] (cv::Scalar actualPhase, cv::Scalar referencePhase, float expected) {
	 // Transfer to the texture, then bind
	EXPECT_TRUE( inputTexture0Float.transferToTexture(cv::Mat(width, height, CV_32FC4, actualPhase)) );
	EXPECT_TRUE( inputTexture1Float.transferToTexture(cv::Mat(width, height, CV_32FC4, referencePhase)) );

	shaderProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	inputTexture0Float.bind( GL_TEXTURE0 );
	inputTexture1Float.bind( GL_TEXTURE1 );

	// Process and check our output
	shaderProcessor.process( );
	IplImage* outputImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 4);
	EXPECT_TRUE(outputTextureFloat.transferFromTexture(outputImage));
	EXPECT_FLOAT_EQ(expected, cv::Mat(outputImage).at<float>(0,0));
  };

  // Now check some values!
  checkValue(cv::Scalar(4.0), cv::Scalar(2.0), 8.0);
  checkValue(cv::Scalar(2.0), cv::Scalar(1.0), 5.0);
  checkValue(cv::Scalar(6.0), cv::Scalar(2.0), 14.0);
}

TEST_F(ShadersTest, Phase2Coordinate)
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

  auto checkValue = [this] ( float actualPhase, cv::Scalar expectedCoordinate ) 
  {
	EXPECT_TRUE( inputTexture0Float.transferToTexture( cv::Mat(width, height, CV_32FC4, actualPhase) ) );


	shaderProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	inputTexture0Float.bind( GL_TEXTURE0 );
	
	// Process and check our output
	shaderProcessor.process( );
	IplImage* outputImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 4);
	EXPECT_TRUE(outputTextureFloat.transferFromTexture(outputImage));
	
	cv::Vec4f actual = cv::Mat(outputImage).at<cv::Vec4f>(0,0);

	EXPECT_FLOAT_EQ(expectedCoordinate(0), actual(0));
	EXPECT_FLOAT_EQ(expectedCoordinate(1), actual(1));
	EXPECT_FLOAT_EQ(expectedCoordinate(2), actual(2));
  };

  checkValue(2.0, cv::Scalar(281.01715, 105.57656, 602.74893));
  checkValue(5.0, cv::Scalar(274.27292, 106.76128, 592.96862));
}

int main(int argc, char **argv)
{
  // Needs to be made so that we can create headless OpenGL stuffs
  QApplication app(argc, argv);

  ::testing::InitGoogleTest(&argc, argv);
  auto testReturn = RUN_ALL_TESTS( );

  system("pause");
  return testReturn;
}