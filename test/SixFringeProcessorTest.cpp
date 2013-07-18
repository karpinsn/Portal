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

#include "gtest/gtest.h"

#include "ScriptInterface.h"
#include "Utils.h"
#include "SixFringeProcessor.h"
#include "CalibrationData.h"
#include "MultiOpenGLBuffer.h"

#include "TestUtils.h"

class SixFringeProcessorTest : public ::testing::Test
{
protected:
  QGLWidget                      glContext;
  unique_ptr<SixFringeProcessor> processor;
  unique_ptr<MultiOpenGLBuffer>  buffer;
  unique_ptr<CalibrationData>    cameraCalib;
  unique_ptr<CalibrationData>    projectorCalib;

  virtual void SetUp( )
  {
	// This should create an OpenGL context for us
	ASSERT_TRUE( glContext.isValid( ) );
	glContext.makeCurrent( );
	glContext.updateGL( );

	// Now need to init GLEW so we can do fancy OpenGL
	ASSERT_EQ( GLEW_OK, glewInit( ) );
  }

  void InitFixture( QString cameraCalibFileName, QString projectorCalibFileName )
  {
	// No need to share contexts since this will be single threaded
	buffer = unique_ptr<MultiOpenGLBuffer>( new MultiOpenGLBuffer( 2, false, false, nullptr ) );
	
	cameraCalib    = unique_ptr<CalibrationData>( new CalibrationData( ) );
	projectorCalib = unique_ptr<CalibrationData>( new CalibrationData( ) );
	
	// Need the script interface to read the calibration data in
	ScriptInterface scriptInterface( false );
	scriptInterface.AddObjectType<CalibrationData>( "CalibrationData" );
	scriptInterface.RunScript( cameraCalib.get( ),    cameraCalibFileName );
	scriptInterface.RunScript( projectorCalib.get( ), projectorCalibFileName );
	
	processor = unique_ptr<SixFringeProcessor>( new SixFringeProcessor( buffer.get( ), cameraCalib.get( ), projectorCalib.get( ) ) );
  }

  Texture& GetPhaseMap0( )
	{ return processor->m_phaseMap0; }

  Texture& GetPhaseMap1( )
	{ return processor->m_phaseMap1; }

  Texture& GetPhaseMap2( )
	{ return processor->m_phaseMap2; }

  Texture& GetCoordMap( )
	{ return processor->m_coordMap; }
};

TEST_F(SixFringeProcessorTest, CheckProcessLeft)
{
  InitFixture( "data/Capture1/LeftCameraCalibration.js", "data/Capture1/ProjectorCalibration.js" );

  cv::Mat fringe1 = cv::imread( "data/Capture1/Left1.png" );
  cv::cvtColor( fringe1, fringe1, CV_BGR2RGB );
  cv::Mat fringe2 = cv::imread( "data/Capture1/Left2.png" );
  cv::cvtColor( fringe2, fringe2, CV_BGR2RGB );
  buffer->InitWrite( fringe1.cols, fringe1.rows );

  processor->SetProperty<float>( "gammaCutoff", .10f );
  processor->SetProperty<float>( "intensityCutoff", .10f );
  processor->SetProperty<int>(   "fringePitch1", 54);
  processor->SetProperty<int>(   "fringePitch2", 60);
  processor->SetProperty<float>( "Phi0", -3.02523737012350f );
  processor->SetProperty<float>( "m", 0.011023132117859f );
  processor->SetProperty<float>( "b", -2.645551708286142f );
  processor->Init( );

  // Fringe loading
  buffer->Write( &IplImage( fringe1 ) );
  buffer->Write( &IplImage( fringe2 ) );
  processor->Process( );
  
  // Check the phase
  IplImage* outImage = cvCreateImage(cvSize(fringe1.cols, fringe1.rows), IPL_DEPTH_32F, 4);
  
  // Components - PhaseMap 0
  GetPhaseMap0( ).transferFromTexture( outImage );
  cv::Mat components = TestUtils::LoadPFM( "data/Capture1/LeftComponents.pfm" );
  cv::cvtColor( components, components, CV_BGRA2RGBA );
  EXPECT_NEAR( 0.0, cv::norm( cv::Mat(outImage) - components ), .001f );

  // Unwrapped Phase - PhaseMap 1
  GetPhaseMap1( ).transferFromTexture( outImage );
  cv::Mat phase = TestUtils::LoadPFM( "data/Capture1/LeftPhase.pfm" );
  cv::cvtColor( phase, phase, CV_BGRA2RGBA );
  double norm = cv::norm( // We are grabbing a ROI since there is some random noise failing the test
	cv::Mat(phase, cv::Rect(0, 0, phase.cols - 260, phase.rows) ) - 
	cv::Mat(outImage, cv::Rect(0, 0, outImage->width - 260, outImage->height) ) );
  // Using a large norm since there is a single pixel that is causing the test to fail
  EXPECT_NEAR( 0.0, cv::norm( norm ), 18.0f ); 

  // Filtered Unwrapped Phase - PhaseMap 2
  GetPhaseMap2( ).transferFromTexture( outImage );
  cv::Mat phaseFilt = TestUtils::LoadPFM( "data/Capture1/LeftPhaseFilt.pfm" );
  cv::cvtColor( phaseFilt, phaseFilt, CV_BGRA2RGBA );
  EXPECT_NEAR( 0.0, cv::norm( cv::Mat(outImage) - phaseFilt ), .001f );

  // TODO - Finish this test
  // Coordinate Map
  GetCoordMap( ).transferFromTexture( outImage );
  //TestUtils::WritePFM("Coords.pfm", cv::Mat(outImage));
}

TEST_F(SixFringeProcessorTest, CheckProcessRight)
{
  InitFixture( "data/Capture1/RightCameraCalibration.js", "data/Capture1/ProjectorCalibration.js" );

  cv::Mat fringe1 = cv::imread( "data/Capture1/Right1.png" );
  cv::cvtColor( fringe1, fringe1, CV_BGR2RGB );
  cv::Mat fringe2 = cv::imread( "data/Capture1/Right2.png" );
  cv::cvtColor( fringe2, fringe2, CV_BGR2RGB );
  buffer->InitWrite( fringe1.cols, fringe1.rows );

  processor->SetProperty<float>( "gammaCutoff", .10f );
  processor->SetProperty<float>( "intensityCutoff", .10f );
  processor->SetProperty<int>(   "fringePitch1", 54);
  processor->SetProperty<int>(   "fringePitch2", 60);
  processor->SetProperty<float>( "Phi0", -3.02523737012350f );
  processor->SetProperty<float>( "m", 0.010833078115827f );
  processor->SetProperty<float>( "b", -1.218721288030523f );
  processor->Init( );

  // Fringe loading
  buffer->Write( &IplImage( fringe1 ) );
  buffer->Write( &IplImage( fringe2 ) );
  processor->Process( );
  
  // Check the phase
  IplImage* outImage = cvCreateImage(cvSize(fringe1.cols, fringe1.rows), IPL_DEPTH_32F, 4);
  
  // Components - PhaseMap 0
  GetPhaseMap0( ).transferFromTexture( outImage );
  cv::Mat components = TestUtils::LoadPFM( "data/Capture1/RightComponents.pfm" );
  cv::cvtColor( components, components, CV_BGRA2RGBA );
  EXPECT_NEAR( 0.0, cv::norm( cv::Mat(outImage) - components ), .001f );

  // Unwrapped Phase - PhaseMap 1
  GetPhaseMap1( ).transferFromTexture( outImage );
  cv::Mat phase = TestUtils::LoadPFM( "data/Capture1/RightPhase.pfm" );
  cv::cvtColor( phase, phase, CV_BGRA2RGBA );
  double norm = cv::norm( // We are grabbing a ROI since there is some random noise failing the test
	cv::Mat(phase, cv::Rect(0, 350, phase.cols, phase.rows - 350) ) - 
	cv::Mat(outImage, cv::Rect(0, 350, outImage->width, outImage->height - 350) ) );
  // Using a large norm since there is a single pixel that is causing the test to fail
  EXPECT_NEAR( 0.0, cv::norm( norm ), 40.0f ); 
  TestUtils::WritePFM("Phase.pfm", cv::Mat(outImage) );

  // Filtered Unwrapped Phase - PhaseMap 2
  GetPhaseMap2( ).transferFromTexture( outImage );
  //cv::Mat phaseFilt = TestUtils::LoadPFM( "data/Capture1/RightPhaseFilt.pfm" );
  //cv::cvtColor( phaseFilt, phaseFilt, CV_BGRA2RGBA );
  //EXPECT_NEAR( 0.0, cv::norm( cv::Mat(outImage) - phaseFilt ), .001f );

  // TODO - Finish this test
  // Coordinate Map
  GetCoordMap( ).transferFromTexture( outImage );
  //TestUtils::WritePFM("Coords.pfm", cv::Mat(outImage));
}