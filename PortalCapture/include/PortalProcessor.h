/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_PORTAL_PROCESSOR_H_
#define _PORTAL_PORTAL_PROCESSOR_H_

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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <wrench/gl/OGLStatus.h>
#include <wrench/gl/ShaderProgram.h>
#include <wrench/gl/Shader.h>
#include <wrench/gl/Texture.h>
#include <wrench/gl/FBO.h>

#include "SixFringeProcessor.h"
#include "TriMesh.h"
#include "CalibrationData.h"

#include "IWriteBuffer.h"
#include "MultiOpenGLBuffer.h"

#include "Utils.h"

using namespace std;
using namespace wrench::gl;

class PortalProcessor : public QGLWidget
{
  Q_OBJECT
	
private:
  DISALLOW_COPY_AND_ASSIGN(PortalProcessor);

  enum OutputMode
  {
	Fringe,
	Depth,
	Holo
  };

  vector<pair<shared_ptr<IProcessContext>, shared_ptr<TriMesh>>> m_captureProcessors;
 
  shared_ptr<IWriteBuffer> m_outputBuffer;

  ShaderProgram m_coordinate2Holo;
  ShaderProgram m_renderTexture;

  Texture	m_encodedMap;

  FBO m_imageProcessor;
  bool m_isInit;

  // Used for specifying a display mode and number (which processor)
  OutputMode  m_displayMode;
  int		  m_displayNumber;

public:
  PortalProcessor(void);
  void AddProcessContext( shared_ptr<IProcessContext> processContext );
  void Init( shared_ptr<IWriteBuffer> outputBuffer );

public slots:
  void OutputFringe( int processorNumber );
  void OutputDepth( int processorNumber );
  void OutputHolo( void );

signals:
  void ProcessedFrame( void );

protected:
  void paintGL( void );

private:
  void _Process( void );
  void _Output( void );
};

#endif	// _PORTAL_PORTAL_PROCESSOR_H_
