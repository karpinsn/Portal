/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_MAIN_CONTROLLER_H_
#define _PORTAL_CAPTURE_MAIN_CONTROLLER_H_

//	Needed so that windows.h does not include Winsock.h
#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif
	#include <WinSock2.h>
	#include <windows.h>
#endif

#ifdef __APPLE__
#include <glew.h>
#include <OpenGL/gl.h>
#elif _WIN32
#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#endif

#include <QObject>
#include <QGLWidget>
#include <memory>
#include <assert.h>

#include "MultiOpenGLBuffer.h"
#include "ISharedGLContextFactory.h"
#include "Utils.h"

#include "Capture\CameraCapture.h"
#include "Process\SixFringeProcessor.h"
#include "Stream\WebsocketStream.h"

using namespace std;

class MainController : public QObject, public ISharedGLContextFactory
{
  Q_OBJECT

private:
  DISALLOW_COPY_AND_ASSIGN(MainController);

  unique_ptr<ICaptureContext>	  m_captureContext;
  unique_ptr<SixFringeProcessor>  m_processContext; //  Main Context : Runs on UI thread
  unique_ptr<IStreamContext>	  m_streamContext;

public:
  MainController();
  void Init(void);

 /**
  * Makes a shared OpenGL context with the main processing context.
  *
  * For some reason if you call this it will invalidate all FBOs and VAOs.
  * That being said, make sure to get all your shared contexts first, and
  * then initalize your components that make use of FBOs and VAOs
  */
  virtual shared_ptr<QGLWidget> MakeSharedContext(void);

public slots:
  void Start(void);
  void Close(void);

signals:
  void Finished(void);
};

#endif	// _PORTAL_CAPTURE_MAIN_CONTROLLER_H_
