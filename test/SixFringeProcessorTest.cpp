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

#include "gtest/gtest.h"

#include "Utils.h"
#include "SixFringeProcessor.h"
#include "CalibrationData.h"
#include "MultiOpenGLBuffer.h"

class SixFringeProcessorTest : public ::testing::Test
{
protected:
  unique_ptr<SixFringeProcessor> processor;

  virtual void SetUp()
  {
	// TODO - Need to fill these
	/*
	auto buffer = make_shared<MultiOpenGLBuffer>( );
	auto cameraCalibration = make_shared<CalibrationData>( );
	auto projectorCalibration = make_shared<CalibrationData>( );

	processor = unique_ptr<SixFringeProcessor>( new SixFringeProcessor( buffer, cameraCalibration, projectorCalibration ) );
	processor->Init( );
	*/
  }
};

TEST_F(SixFringeProcessorTest, CheckProcess)
{
  // TODO - Need to set the correct values in our MultiBuffer
  //processor->Process( );
  // TODO - Need to check the output
}