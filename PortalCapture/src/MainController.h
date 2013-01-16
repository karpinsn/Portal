/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_MAIN_CONTROLLER_H_
#define _PORTAL_CAPTURE_MAIN_CONTROLLER_H_

#include <QObject>
#include <QGLWidget>
#include <QGLPixelBuffer>
#include <QGLFramebufferObject>
#include <memory>
#include <assert.h>

#include "OpenGLTripleBuffer.h"
#include "ISharedGLContextFactory.h"
#include "Utils.h"
#include "Capture\CameraCapture.h"

using namespace std;

class MainController : public QObject, public ISharedGLContextFactory
{
  Q_OBJECT

private:
  DISALLOW_COPY_AND_ASSIGN(MainController);

  unique_ptr<QGLWidget> m_mainContext;

  unique_ptr<ICaptureContext> m_captureContext;
  //  IDecodeContext
  //  IStreamContext

public:
  MainController();
  void Init(void);

  virtual shared_ptr<QGLWidget> MakeSharedContext(void);

public slots:
  void Start(void);
  void Close(void);

signals:
  void Finished(void);
};

#endif	// _PORTAL_CAPTURE_MAIN_CONTROLLER_H_
