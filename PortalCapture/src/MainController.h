/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_MAIN_CONTROLLER_H_
#define _PORTAL_CAPTURE_MAIN_CONTROLLER_H_

#include <QObject>
#include <QGLContext>
#include <memory>

#include "Capture\CameraCapture.h"

using namespace std;

class MainController : public QObject
{
  Q_OBJECT

private:
  unique_ptr<QGLContext> m_mainContext;

  unique_ptr<ICaptureContext> m_captureContext;
  //  IDecodeContext
  //  IStreamContext

public:
  MainController();
  //  TODO: Add in parameters we need. Output port, camera identifer, etc.
  void Init(void);

public slots:
  void Start(void);
  void Close(void);

signals:
  void Finished(void);
};

#endif	// _PORTAL_CAPTURE_MAIN_CONTROLLER_H_
