/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_CAMERA_CAPTURE_H_
#define _PORTAL_CAPTURE_CAMERA_CAPTURE_H_

#include <QObject>
#include <QThread>

class ICaptureContext
{
public:
  virtual void Init() = 0;
  virtual void Start() = 0;
};

class CameraCaptureWorker : public QObject
{
  Q_OBJECT

private:
  bool m_running;

public:
  void Init(void);
  void Stop(void);

signals:
  void Done(void);

public slots:
  void Capture(void);
};

class CameraCapture : public QObject, public ICaptureContext
{
  Q_OBJECT
	
//	Need to create a lens::Camera in initialize
//	Need a pointer to a triplebuffer
private:
  QThread* m_workerThread;
  CameraCaptureWorker* m_worker;

public:
  void Init();
  void Start();
};

#endif	// _PORTAL_CAPTURE_CAMERA_CAPTURE_H_
