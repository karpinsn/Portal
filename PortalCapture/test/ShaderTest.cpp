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
  const static int	  width = 64;
  const static int	  height = 64;
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

  virtual void CheckValue(cv::Scalar ExpectedOut, cv::Scalar texture0 = cv::Scalar(0.0), cv::Scalar texture1 = cv::Scalar(0.0))
  {
	// Transfer to the texture, then bind
	EXPECT_TRUE( inputTexture0Float.transferToTexture(cv::Mat(width, height, CV_32FC4, texture0)) );
	EXPECT_TRUE( inputTexture1Float.transferToTexture(cv::Mat(width, height, CV_32FC4, texture1)) );

	shaderProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	inputTexture0Float.bind( GL_TEXTURE0 );
	inputTexture1Float.bind( GL_TEXTURE1 );
	
	shaderProcessor.process( );
	IplImage* outputImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 4);
	EXPECT_TRUE(outputTextureFloat.transferFromTexture(outputImage));

	// Check the rgba value at 0,0
	cv::Vec4f actual = cv::Mat(outputImage).at<cv::Vec4f>(0,0);
	for(int i = 0; i < 4; ++i)
	{ 
	  //EXPECT_FLOAT_EQ(ExpectedOut(i), actual(i));
	  EXPECT_NEAR(ExpectedOut(i), actual(i), .0001f); 
	}
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

  // Now check some values!
  CheckValue( cv::Scalar(113.4973f, 113.4973f, 113.4973f, 113.4973f), cv::Scalar(.4f, 0.0, 0.0, 0.0),	cv::Scalar(0.0f, 0.0, 5.6832, 0.0));
  CheckValue( cv::Scalar(2.0f, 2.0f, 2.0f, 2.0f),					  cv::Scalar(2.0f, 0.0, 0.0, 0.0),	cv::Scalar(0.0f, 0.0, .2, 0.0));
  CheckValue( cv::Scalar(95.04777f, 95.04777f, 95.04777f, 95.04777f), cv::Scalar(.8f, 0.0, 0.0, 0.0),	cv::Scalar(0.0f, 0.0, 4.7832, 0.0)); 
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

  // Now check some values!
  CheckValue( cv::Scalar(8.0, 8.0, 8.0, 8.0),	  cv::Scalar(4.0), cv::Scalar(2.0) );
  CheckValue( cv::Scalar(5.0, 5.0, 5.0, 5.0),	  cv::Scalar(2.0), cv::Scalar(1.0) );
  CheckValue( cv::Scalar(14.0, 14.0, 14.0, 14.0), cv::Scalar(6.0), cv::Scalar(2.0) );
}

TEST_F(ShadersTest, Fringe2WrappedPhase)
{
  wrench::gl::ShaderProgram shader;
  shader.init();
  shader.attachShader(new wrench::gl::Shader(GL_VERTEX_SHADER, "Shaders/PassThrough.vert"));
  shader.attachShader(new wrench::gl::Shader(GL_FRAGMENT_SHADER, "Shaders/Fringe2WrappedPhase.frag"));
  shader.bindAttributeLocation("vert", 0);
  shader.bindAttributeLocation("vertTexCoord", 1);
  shader.link();
  shader.uniform("fringeImage1", 0);
  shader.uniform("fringeImage2", 1);
  shader.uniform("pitch1", 60);
  shader.uniform("pitch2", 63);

  shaderProcessor.bind( );
  shader.bind( );

  // Now check some values!
  CheckValue( cv::Scalar(-2.5131, 2.2194, 1.5507),	cv::Scalar(0.2078, 0.1020, 0.3608), cv::Scalar(0.3686, 0.1373, 0.1686) );
  CheckValue( cv::Scalar(2.8452, 1.2324, 1.6127),	cv::Scalar(0.2431, 0.0863, 0.1961), cv::Scalar(0.2392, 0.2078, 0.0784) );
  CheckValue( cv::Scalar(2.2570, -0.0890, 2.3461),	cv::Scalar(0.3294, 0.1255, 0.1608), cv::Scalar(0.1333, 0.3333, 0.1529) );
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

  // Now check some values!
  CheckValue( cv::Scalar(281.01715f, 105.57656f, 602.74893f), cv::Scalar(2.0f) );
  CheckValue( cv::Scalar(274.27292f, 106.76128f, 592.96862f), cv::Scalar(5.0f) );
}

TEST_F(ShadersTest, Coordinate2Holo)
{

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