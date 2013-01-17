#include "CameraCapture.h"

CameraCaptureWorker::CameraCaptureWorker(shared_ptr<IWriteBuffer> outputBuffer) : 
  m_running(false), m_currentChannelLoad(0), m_outputBuffer(outputBuffer)
{ }

void CameraCaptureWorker::Start( )
{
  m_camera = make_shared<lens::OpenCVCamera>();
  m_camera->init();

  //  TODO: Not sure how we want to know we want a 3 channel image
  m_packFrame = shared_ptr<IplImage>(
	cvCreateImage( cvSize( m_camera->getWidth( ), m_camera->getHeight( ) ), IPL_DEPTH_8U, 3 ), 
	[]( IplImage* ptr ) { cvReleaseImage( &ptr ); } );

  m_outputBuffer->initWrite(m_camera->getWidth(), m_camera->getHeight());
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
	IplImage* frame = m_camera->getFrame();
	
	//	Pack our camera image
	cvSetImageCOI(frame, 1);
	cvSetImageCOI(m_packFrame.get(), ( m_currentChannelLoad + 1 ) );
	cvCopy(frame, m_packFrame.get());
	cvSetImageCOI(frame, 0 );
	cvSetImageCOI(m_packFrame.get(), 0 );

	//	If we have all the channels packed then we need to transfer
	m_currentChannelLoad++;
	if( 3 == m_currentChannelLoad )
	{
	  m_currentChannelLoad = 0;
	  m_outputBuffer->write( m_packFrame.get( ) );
	}
  }

  emit Done();
}

CameraCapture::CameraCapture(void)
{ }

#include <QTimer>
void CameraCapture::Init(shared_ptr<IWriteBuffer> outputBuffer)
{
  //  TODO: Make sure we only call this once
  m_workerThread = new QThread();
  m_worker = new CameraCaptureWorker( outputBuffer );

  //  Init must be called after moving to the thread so that
  //  everything is created in the threads' memory space
  m_worker->moveToThread(m_workerThread);

  //  TODO: Might have to call this through SIG/SLOT so that it is on that thread
  //QTimer::singleShot(0, m_worker, SLOT( Init( outputBuffer ) ) );
  //QMetaObject::invokeMethod(m_worker, "Init", Qt::QueuedConnection);
  //m_worker->Init(outputBuffer);

  connect(m_workerThread, SIGNAL( started( ) ),	  m_worker,		  SLOT( Start( ) ));
  connect(m_worker,		  SIGNAL( Done( ) ),	  m_workerThread, SLOT( quit( ) ));
  connect(m_workerThread, SIGNAL( finished( ) ),  m_workerThread, SLOT( deleteLater( ) ));
  connect(m_worker,		  SIGNAL( Done( ) ),	  m_worker,		  SLOT( deleteLater( ) ));

  m_workerThread->start();
}

void CameraCapture::Start(void)
{ 
  QTimer::singleShot(0, m_worker, SLOT( Capture( ) ) );
}