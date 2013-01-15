#include "CameraCapture.h"

void CameraCaptureWorker::Init(void)
{
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

  }

  emit Done();
}

void CameraCapture::Init(void)
{
  //  TODO: Make sure we only call this once
  m_workerThread = new QThread();
  m_worker = new CameraCaptureWorker();

  //  Init must be called after moving to the thread so that
  //  everything is created in the threads' memory space
  m_worker->moveToThread(m_workerThread);
  m_worker->Init();

  connect(m_workerThread, SIGNAL( started() ), m_worker, SLOT( Capture() ));
  connect(m_worker, SIGNAL( Done() ), m_workerThread, SLOT( quit() ));
  connect(m_workerThread, SIGNAL( finished() ), m_workerThread, SLOT( deleteLater() ));
  connect(m_worker, SIGNAL( Done() ), m_worker, SLOT( deleteLater() ));
}

void CameraCapture::Start(void)
{
}