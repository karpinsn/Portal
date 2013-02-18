#include "CameraCapture.h"

CameraCaptureWorker::CameraCaptureWorker(shared_ptr<IWriteBuffer> outputBuffer) : 
  m_running(false), m_currentChannelLoad(0), m_outputBuffer(outputBuffer), m_dropFrame(false)
{ }

bool CameraCaptureWorker::IsRunning( void )
{
  return m_running;
}

void CameraCaptureWorker::Init( void )
{
  m_camera = make_shared<lens::OpenCVCamera>();
  //m_camera = make_shared<lens::FileCamera>();
  //m_camera = make_shared<lens::PointGreyCamera>();
  m_camera->init();
  m_camera->open();

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

void CameraCaptureWorker::DropFrame( void )
{
  wrench::Logger::logDebug("Dropping a frame");
  m_dropFrame = true;
}

void CameraCaptureWorker::Capture()
{
  while(m_running)
  {
	//	Grab our image
	IplImage* frame = m_camera->getFrame();
	
	//	Check if we should drop this frame
	if(m_dropFrame)
	{ 
	  m_dropFrame = false;
	  continue; 
	} 

	// PROFILE-TODO: Copying is a "hot point" in the code. 
	//	Pack our camera image
	_OpenCVPack(frame);
	//_FastPack(frame);

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

void CameraCaptureWorker::_OpenCVPack(IplImage* src)
{
  //  Pack using OpenCV channel copying
  cvSetImageCOI(src, 1);
  cvSetImageCOI(m_packFrame.get(), ( m_currentChannelLoad + 1 ) );
  cvCopy(src, m_packFrame.get());
  cvSetImageCOI(src, 0 );
  cvSetImageCOI(m_packFrame.get(), 0 );
}

void CameraCaptureWorker::_FastPack(IplImage* src)
{
  int height = src->height;
  int colStart = src->width + 1; // Start at plus 1 so we can predecrement

  int srcWidthStep = src->widthStep;
  int destWidthStep = m_packFrame->widthStep;
  char* destPtr = m_packFrame->imageData;

  int rowIndex = height + 1;	// Start at plus 1 so we can predecrement
  while(--rowIndex)
  {
	char* srcDataPtr = &(src->imageData[(height - rowIndex) * srcWidthStep]);
	char* destDataPtr = &(destPtr[(height - rowIndex) * destWidthStep + m_currentChannelLoad]);
	
	int colIndex = colStart;	
	while(--colIndex)
	{
	  *destDataPtr = *srcDataPtr;
	  destDataPtr += 3;
	  srcDataPtr += 3;
	}
  }
}

CameraCapture::CameraCapture(shared_ptr<IWriteBuffer> outputBuffer)
{
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
  m_workerThread->start( QThread::TimeCriticalPriority );
}

void CameraCapture::DropFrame( void )
{
  m_worker->DropFrame();
}