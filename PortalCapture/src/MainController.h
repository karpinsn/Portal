/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_MAIN_CONTROLLER_H_
#define _PORTAL_CAPTURE_MAIN_CONTROLLER_H_

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
#include <QGLContext>
#include <memory>
#include <assert.h>

#include "MultiOpenGLBuffer.h"
#include "ISharedGLContextFactory.h"
#include "Utils.h"

#include "Capture\CameraCapture.h"
#include "Process\SixFringeProcessor.h"

using namespace std;

class MainController : public QObject, public ISharedGLContextFactory
{
  Q_OBJECT

private:
  DISALLOW_COPY_AND_ASSIGN(MainController);

  unique_ptr<ICaptureContext> m_captureContext;
  //  Main Context : Runs on UI thread
  unique_ptr<SixFringeProcessor> m_processContext;
  //  IStreamContext

public:
  MainController();
  void Init(void);

  virtual shared_ptr<QGLContext> MakeSharedContext(void);

public slots:
  void Start(void);
  void Close(void);

signals:
  void Finished(void);
};

#endif	// _PORTAL_CAPTURE_MAIN_CONTROLLER_H_
