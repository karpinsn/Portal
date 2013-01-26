/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_CAMERA_CAPTURE_H_
#define _PORTAL_CAPTURE_CAMERA_CAPTURE_H_

#include <memory>

#include <QObject>
#include <QThread>
#include <QTimer>

#include <cv.h>
#include <highgui.h>

#include <Lens\FileCamera.h>
#include <Lens\OpenCVCamera.h>
#include <Lens\CameraObserver.h>

#include "../Utils.h"
#include "../IWriteBuffer.h"

using namespace std;

class ICaptureContext : public QObject
{
  Q_OBJECT

public:
  virtual void Init( shared_ptr<IWriteBuffer> outputBuffer ) = 0;
  virtual void Start( ) = 0;

public slots:
  virtual void DropFrame( ) = 0;
};

class CameraCaptureWorker : public QObject
{
  Q_OBJECT
  

private:
  DISALLOW_COPY_AND_ASSIGN(CameraCaptureWorker);
  bool m_running;
  int m_currentChannelLoad;
  bool m_dropFrame;

  shared_ptr<IWriteBuffer> m_outputBuffer;
  shared_ptr<lens::Camera> m_camera;
  shared_ptr<IplImage> m_packFrame;

public:
  CameraCaptureWorker(shared_ptr<IWriteBuffer> outputBuffer);
  bool IsRunning( void );

signals:
  void Done( void );

public slots:
  void Init( void );
  void Stop( void );
  void DropFrame ( void );
  void Capture( void );
};

class CameraCapture : public ICaptureContext
{
  Q_OBJECT
	
private:
  DISALLOW_COPY_AND_ASSIGN(CameraCapture);
  QThread* m_workerThread;
  CameraCaptureWorker* m_worker;

public:
  CameraCapture(void) { };
  void Init(shared_ptr<IWriteBuffer> outputBuffer);
  void Start();

public slots:
  void DropFrame( void );
};

#endif	// _PORTAL_CAPTURE_CAMERA_CAPTURE_H_
