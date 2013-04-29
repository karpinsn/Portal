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
#include "SplatField.h"
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

  vector<pair<shared_ptr<IProcessContext>, shared_ptr<SplatField>>> m_captureProcessors;
 
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
 /**
  * Outputs the fringe from the specified processor whenever _Output() is called.
  */
  void OutputFringe( int processorNumber );

 /**
  * Outputs the depthmap from the specified processor whenever _Output() is called.
  */
  void OutputDepth( int processorNumber );

 /**
  * Outputs the holoencoded frame whenever _Output() is called. This is the default output 
  */
  void OutputHolo( void );

signals:
 /**
  * Signal that is emitted once a frame has been processed
  */
  void ProcessedFrame( void );

protected:
  void paintGL( void );

private:
 /**
  * Processes all of the capture processors into a single holo frame.
  * This should only be called from the draw thread.
  */
  void _Process( void );

 /**
  * Outputs whatever the currently selected output is to the output buffer
  */
  void _Output( void );
};

#endif	// _PORTAL_PORTAL_PROCESSOR_H_
