/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_PROCESS_SIX_FRINGE_PROCESSOR_H_
#define _PORTAL_PROCESS_SIX_FRINGE_PROCESSOR_H_

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
#include <vector>

#include <QObject>
#include <QGLWidget>
#include <QThread>

#include <wrench/gl/OGLStatus.h>
#include <wrench/gl/ShaderProgram.h>
#include <Wrench/gl/GaussProgram.h>
#include <wrench/gl/Shader.h>
#include <wrench/gl/Texture.h>
#include <wrench/gl/FBO.h>

#include "DynamicallyScriptableQObject.h"
#include "CalibrationData.h"

#include "IWriteBuffer.h"
#include "MultiOpenGLBuffer.h"

#include "Utils.h"

using namespace std;
using namespace wrench::gl;

class IProcessContext
{
public:
  virtual void       Init( void ) = 0;
  virtual const int  GetWidth( void ) = 0;
  virtual const int  GetHeight( void ) = 0;
  virtual void       BindDepthMap( GLenum texture ) = 0;
  virtual void       BindFringeImage( GLenum texture ) = 0;
  virtual void       Process( void ) = 0;
};

class SixFringeProcessor : public DynamicallyScriptableQObject, public IProcessContext
{
  Q_OBJECT
	
private:
  DISALLOW_COPY_AND_ASSIGN(SixFringeProcessor);

  shared_ptr<MultiOpenGLBuffer> m_inputBuffer;
  shared_ptr<CalibrationData>	m_cameraCalibration;
  shared_ptr<CalibrationData>	m_projectorCalibration;
  
  ShaderProgram m_fringe2Phase;
  ShaderProgram m_phaseFilter;
  ShaderProgram m_gammaEroder;
  GaussProgram	m_gaussFilter;
  ShaderProgram m_wrapped2Unwrapped;
  ShaderProgram m_phase2Depth;

  Texture	m_phaseMap0;
  Texture	m_phaseMap1;
  Texture	m_phaseMap2;
  Texture	m_depthMap;

  FBO m_imageProcessor;
  bool m_isInit;

public:
  SixFringeProcessor( shared_ptr<MultiOpenGLBuffer> inputBuffer, 
					  shared_ptr<CalibrationData> cameraCalibration, 
					  shared_ptr<CalibrationData> projectorCalibration );
  void      Init( void );
  const int GetWidth( void );
  const int GetHeight( void );
  void      BindDepthMap( GLenum texture );
  void      BindFringeImage( GLenum texture );
  void      Process( void );

signals:
  void ProcessedFrame( void );

private:
  void _wrapPhase( GLenum drawBuffer, shared_ptr<MultiOpenGLBuffer> fringeBuffer );
  void _filterPhase( GLenum drawBuffer, Texture& phase2Filter );
  void _erodeGamma( GLenum drawBuffer, Texture& gamma2Filter );
  void _gaussianFilter( GLenum pass1DrawBuffer, GLenum pass2DrawBuffer, Texture& pass1ReadBuffer, Texture& pass2ReadBuffer );
  void _unwrapPhase( GLenum drawBuffer, Texture& unfilteredPhase, Texture& filteredPhase );
  void _calculateDepth( GLenum drawBuffer, Texture& phase );
};

#endif	// _PORTAL_PROCESS_SIX_FRINGE_PROCESSOR_H_
