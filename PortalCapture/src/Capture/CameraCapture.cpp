#include "CameraCapture.h"

CameraCaptureWorker::CameraCaptureWorker(void)
{ }

void CameraCaptureWorker::Init(shared_ptr<IWriteBuffer> outputBuffer)
{
  m_outputBuffer = outputBuffer;
  //  TODO: Get the actual width and height from the camera
  m_outputBuffer->initWrite(16, 16);

  m_running = true;
}

void CameraCaptureWorker::Stop(void)
{
  m_running = false;
}

void CameraCaptureWorker::Capture()
{
  while(m_running)
  {
	//	TODO: Grab from the camera, pack, and then write if nessacary
	//m_outputBuffer->write(0, 0);
  }

  emit Done();
}

CameraCapture::CameraCapture(void)
{ }

void CameraCapture::Init(shared_ptr<IWriteBuffer> outputBuffer)
{
  //  TODO: Make sure we only call this once
  m_workerThread = new QThread();
  m_worker = new CameraCaptureWorker();

  //  Init must be called after moving to the thread so that
  //  everything is created in the threads' memory space
  m_worker->moveToThread(m_workerThread);

  //  TODO: Might have to call this through SIG/SLOT so that it is on that thread
  m_worker->Init(outputBuffer);

  connect(m_workerThread, SIGNAL( started( ) ),	  m_worker,		  SLOT( Capture( ) ));
  connect(m_worker,		  SIGNAL( Done( ) ),	  m_workerThread, SLOT( quit( ) ));
  connect(m_workerThread, SIGNAL( finished( ) ),  m_workerThread, SLOT( deleteLater( ) ));
  connect(m_worker,		  SIGNAL( Done( ) ),	  m_worker,		  SLOT( deleteLater( ) ));
}

void CameraCapture::Start(void)
{ 
  m_workerThread->start();
}