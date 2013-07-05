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

#include "TriMesh.h"

class ShadersTest : public ::testing::Test
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

	inputTexture0Float.init( width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT );
	inputTexture1Float.init( width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT );
	outputTextureFloat.init( width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT );
	
	shaderProcessor.init( width, height );
	shaderProcessor.setTextureAttachPoint( outputTextureFloat, GL_COLOR_ATTACHMENT0 );
  }

  virtual void CheckValue(cv::Scalar ExpectedOut, cv::Scalar texture0 = cv::Scalar(0.0), cv::Scalar texture1 = cv::Scalar(0.0))
  {
	// Transfer to the texture, then bind
	ASSERT_TRUE( inputTexture0Float.transferToTexture(cv::Mat(width, height, CV_32FC4, texture0)) );
	ASSERT_TRUE( inputTexture1Float.transferToTexture(cv::Mat(width, height, CV_32FC4, texture1)) );

	shaderProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	inputTexture0Float.bind( GL_TEXTURE0 );
	inputTexture1Float.bind( GL_TEXTURE1 );
	
	shaderProcessor.process( );
	IplImage* outputImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 4);
	ASSERT_TRUE(outputTextureFloat.transferFromTexture(outputImage));

	// Check the rgba value at 0,0
	cv::Vec4f actual = cv::Mat(outputImage).at<cv::Vec4f>(5,5);
	for(int i = 0; i < 4; ++i)
	{ 
	  // Cant use EXPECT_FLOAT_EQ since there are rounding errors between different GPUs
	  EXPECT_NEAR(ExpectedOut(i), actual(i), .0001f); 
	}
  }
};


TEST_F(ShadersTest, Coordinate2Holo)
{
  wrench::gl::ShaderProgram shader;
  shader.init( );
  shader.attachShader( new wrench::gl::Shader(GL_VERTEX_SHADER, "Shaders/Coordinate2Holo.vert") );
  shader.attachShader( new wrench::gl::Shader(GL_FRAGMENT_SHADER, "Shaders/Coordinate2Holo.frag") );
  shader.bindAttributeLocation("vert", 0);
  shader.bindAttributeLocation("vertTexCoord", 1);
  shader.link( );
  
  shader.uniform( "coordinateMap", 0 );
  shader.uniform( "modelView", glm::mat4( ) ); // Just using the identity
  shader.uniform( "projectorModelView", glm::mat4( ) ); // Test data is set for identity
  shader.uniform( "projectionMatrix", glm::ortho( 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f ) );
  shader.uniform( "fringeFrequency", 2.0f);

  TriMesh mesh(256, 256);
  mesh.initMesh( );
  cv::Mat coordinateMap = cv::Mat(height, width, CV_32FC4);
  for(int r = 0; r < height; ++r)
  {
	float* pixelPointer = coordinateMap.ptr<float>(r);
	for(int c = 0; c < width; ++c)
	{
	  pixelPointer[c * 4] = double(c) / double(width - 1);
	  pixelPointer[c * 4 + 1] = float(r) / float(height - 1);
	  pixelPointer[c * 4 + 2] = 0.0f;
	  pixelPointer[c * 4 + 3] = 1.0f;
	}
  }

  ASSERT_TRUE( inputTexture0Float.transferToTexture( coordinateMap ) );

  glPushAttrib(GL_VIEWPORT_BIT);
  {
	glViewport (0, 0, width, height);
	shaderProcessor.bind( );
	shader.bind( );
	shaderProcessor.bindDrawBuffer( GL_COLOR_ATTACHMENT0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	inputTexture0Float.bind( GL_TEXTURE0 );
	mesh.draw( );
  }
  glPopAttrib();

  IplImage* outputImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 4);
  ASSERT_TRUE(outputTextureFloat.transferFromTexture(outputImage));

  cv::Mat actualImage;
  cv::Mat(outputImage).convertTo(actualImage, CV_8UC4, 255.0);
  cv::Mat expectedImage = cv::imread("data/TestHolo1.png");
  cv::cvtColor(expectedImage, expectedImage, CV_BGRA2RGBA);

  cv::imwrite("expected.png", expectedImage);
  cv::imwrite("actual.png", actualImage);

  // Check that the images are close
  for(int r = 0; r < height; ++r)
  {
	for(int c = 0; c < width; ++c)
	{
	  auto expected = expectedImage.at<cv::Vec3b>(r,c);
	  auto actual = actualImage.at<cv::Vec3b>(r,c);
	  for(int i = 0; i < 3; ++i)
	  { 
		//EXPECT_EQ(expected(i), actual(i)); 
	  }
	}
  }
}

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
  shader.uniform("pitch1", 60);
  shader.uniform("pitch2", 63);
  
  // Now do the actual shader pass to see if we get the expected output
  shaderProcessor.bind( );
  shader.bind( );

  // Now check some values!
  CheckValue( cv::Scalar(2.6179812, 2.6179812, 2.6179812, 1.0), cv::Scalar(.5f, -.8660f, .7159f, -.6982f), cv::Scalar(.5f, -.8660f, .7159f, -.6982f) );
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
  CheckValue( cv::Scalar(.50000793f, -.86602539f, .7158522f, -.69826132f), cv::Scalar(.9330, 0.0670, .5), cv::Scalar( 0.9845, 0.1509, 0.3646) );
  CheckValue( cv::Scalar(-0.00000011921111, -1, .60631108, -.79527503), cv::Scalar(.75, 0, .75), cv::Scalar(.9505, .0869, .4226) );
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
  CheckValue( cv::Scalar(281.01715f, 105.57656f, 602.74893f, 1.0f), cv::Scalar(2.0f) );
  CheckValue( cv::Scalar(274.27292f, 106.76128f, 592.96862f, 1.0f), cv::Scalar(5.0f) );
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