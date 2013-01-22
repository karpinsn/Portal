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

#include <QObject>
#include <QGLWidget>
#include <QThread>

#include <wrench/gl/OGLStatus.h>
#include <wrench/gl/ShaderProgram.h>
#include <wrench/gl/Shader.h>
#include <wrench/gl/Texture.h>
#include <wrench/gl/FBO.h>

#include "../IWriteBuffer.h"
#include "../IOpenGLReadBuffer.h"

#include "../Utils.h"

using namespace std;
using namespace wrench::gl;

class IProcessContext
{
public:
  virtual void Init( shared_ptr<IOpenGLReadBuffer> inputBuffer, shared_ptr<IOpenGLWriteBuffer> outputBuffer ) = 0;
};

class SixFringeProcessor : public QGLWidget, public IProcessContext
{
  Q_OBJECT
	
private:
  DISALLOW_COPY_AND_ASSIGN(SixFringeProcessor);
  shared_ptr<IOpenGLReadBuffer> m_inputBuffer;
  shared_ptr<IOpenGLWriteBuffer> m_outputBuffer;

  ShaderProgram m_phaseCalculator;
  ShaderProgram m_phaseFilter;
  ShaderProgram m_depthCalculator;

  Texture m_phaseMap0;
  Texture m_phaseMap1;
  //Texture m_depthMap;
  Texture m_referencePhase;

  FBO m_imageProcessor;
  bool m_isInit;
  bool m_captureReference;

public:
  SixFringeProcessor(void);
  void Init( shared_ptr<IOpenGLReadBuffer> inputBuffer, shared_ptr<IOpenGLWriteBuffer> outputBuffer );

protected:
  void paintGL( void );

private:
  void _calculatePhase( GLenum drawBuffer );
  void _filterPhase( GLenum drawBuffer, Texture& phase2Filter );
  void _calculateDepth( GLenum drawBuffer, Texture& phase );
};

#endif	// _PORTAL_PROCESS_SIX_FRINGE_PROCESSOR_H_
