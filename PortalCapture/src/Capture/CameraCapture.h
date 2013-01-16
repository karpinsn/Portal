/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_CAMERA_CAPTURE_H_
#define _PORTAL_CAPTURE_CAMERA_CAPTURE_H_

#include <memory>

#include <QObject>
#include <QThread>

#include <Lens\OpenCVCamera.h>

#include "../Utils.h"
#include "../IWriteBuffer.h"

using namespace std;

class ICaptureContext
{
public:
  virtual void Init(shared_ptr<IWriteBuffer> outputBuffer) = 0;
  virtual void Start() = 0;
};

class CameraCaptureWorker : public QObject
{
  Q_OBJECT

private:
  DISALLOW_COPY_AND_ASSIGN(CameraCaptureWorker);
  bool m_running;

  shared_ptr<IWriteBuffer> m_outputBuffer;

public:
  CameraCaptureWorker(void);
  void Init(shared_ptr<IWriteBuffer> outputBuffer);
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
  DISALLOW_COPY_AND_ASSIGN(CameraCapture);
  QThread* m_workerThread;
  CameraCaptureWorker* m_worker;

public:
  CameraCapture(void);
  void Init(shared_ptr<IWriteBuffer> outputBuffer);
  void Start();
};

#endif	// _PORTAL_CAPTURE_CAMERA_CAPTURE_H_
