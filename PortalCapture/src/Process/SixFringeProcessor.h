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

#include "CalibrationData.h"

#include "../IWriteBuffer.h"
#include "../MultiOpenGLBuffer.h"

#include "../Utils.h"

using namespace std;
using namespace wrench::gl;

class IProcessContext
{
public:
  virtual void Init( void ) = 0;
  virtual void BindDepthMap( GLenum texture ) = 0;
  virtual void BindFringeImage( GLenum texture ) = 0;
  virtual void Process( void ) = 0;
  virtual void CaptureReference( void ) = 0;
};

class SixFringeProcessor : public QObject, public IProcessContext
{
  Q_OBJECT
	
private:
  DISALLOW_COPY_AND_ASSIGN(SixFringeProcessor);

  shared_ptr<MultiOpenGLBuffer> m_inputBuffer;
  shared_ptr<CalibrationData>	m_calibrationData;
  
  ShaderProgram m_fringe2Phase;
  ShaderProgram m_phaseFilter;
  GaussProgram m_gaussFilter;
  ShaderProgram m_phase2Depth;

  Texture	m_phaseMap0;
  Texture	m_phaseMap1;
  Texture	m_referencePhase;
  Texture	m_depthMap;

  FBO m_imageProcessor;
  bool m_isInit;
  bool m_captureReference;

  float m_shift;
  float m_scale;

public:
  SixFringeProcessor( shared_ptr<MultiOpenGLBuffer> inputBuffer, shared_ptr<CalibrationData> calibrationData );
  void Init( void );
  void BindDepthMap( GLenum texture );
  void BindFringeImage( GLenum texture );
  void Process( void );
  void CaptureReference( void );

public slots:
  void SetScale( float scale );
  void SetShift( float shift );

signals:
  void ProcessedFrame( void );

private:
  void _calculatePhase( GLenum drawBuffer, shared_ptr<MultiOpenGLBuffer> fringeBuffer );
  void _filterPhase( GLenum drawBuffer, Texture& phase2Filter );
  void _calculateDepth( GLenum drawBuffer, Texture& phase );
};

#endif	// _PORTAL_PROCESS_SIX_FRINGE_PROCESSOR_H_
