#include "CameraCapture.h"

CameraCaptureWorker::CameraCaptureWorker(shared_ptr<IWriteBuffer> outputBuffer) : 
  m_running(false), m_currentChannelLoad(0), m_outputBuffer(outputBuffer)
{ }

bool CameraCaptureWorker::IsRunning( void )
{
  return m_running;
}

void CameraCaptureWorker::Init( void )
{
  m_camera = make_shared<lens::OpenCVCamera>();
  m_camera->init();

  //  TODO: Not sure how we want to know we want a 3 channel image
  m_packFrame = shared_ptr<IplImage>(
	cvCreateImage( cvSize( m_camera->getWidth( ), m_camera->getHeight( ) ), IPL_DEPTH_8U, 3 ), 
	[]( IplImage* ptr ) { cvReleaseImage( &ptr ); } );

  m_outputBuffer->InitWrite(m_camera->getWidth(), m_camera->getHeight());
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
	//	Grab our image
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
	  m_outputBuffer->Write( m_packFrame.get( ) );
	}
  }

  emit Done();
}

void CameraCapture::Init(shared_ptr<IWriteBuffer> outputBuffer)
{
  //  TODO: Make sure we only call this once
  m_workerThread = new QThread();
  m_worker = new CameraCaptureWorker( outputBuffer );
  m_worker->moveToThread(m_workerThread);

  m_worker->Init( );

  connect(m_workerThread, SIGNAL( started( ) ),	  m_worker,		  SLOT( Capture( ) ));
  connect(m_worker,		  SIGNAL( Done( ) ),	  m_workerThread, SLOT( quit( ) ));
  connect(m_workerThread, SIGNAL( finished( ) ),  m_workerThread, SLOT( deleteLater( ) ));
  connect(m_worker,		  SIGNAL( Done( ) ),	  m_worker,		  SLOT( deleteLater( ) ));
}

void CameraCapture::Start(void)
{ 
  m_workerThread->start();
}