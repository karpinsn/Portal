#include "WebsocketStream.h"

WebsocketStream::WebsocketStream(int port, shared_ptr<IReadBuffer> inputBuffer)
{
  // Open our socket connection
  m_socket.start(port);

  //	Start our background thread that the socket can broadcast on
  m_socketProcessorThread = new QThread(this);
  m_socketProcessor = new WebsocketProcessor(m_socket);
  m_socketProcessor->moveToThread(m_socketProcessorThread);

  //  Connect the thread and its timer 
  connect(m_socketProcessorThread, SIGNAL(started()),  m_socketProcessor, SLOT(ProcessSocket()));
  connect(m_socketProcessor, SIGNAL(Finished()), m_socketProcessorThread, SLOT(quit()));
  connect(m_socketProcessorThread, SIGNAL(finished()), m_socketProcessorThread, SLOT(deleteLater()));
  connect(m_socketProcessor, SIGNAL(Finished()), m_socketProcessor, SLOT(deleteLater())); 

  //	Start our background thread that will process the output stream
  m_streamProcessorThread = new QThread(this);
  m_socketStreamer = new WebsocketStreamer(m_socket, inputBuffer);
  m_socketStreamer->moveToThread(m_streamProcessorThread);
  m_socketStreamer->Init( );

  //	Connect the thread and its timer
  connect(m_socketStreamer, SIGNAL(Finished()), m_streamProcessorThread, SLOT(quit()));
  connect(m_streamProcessorThread, SIGNAL(finished()), m_streamProcessorThread, SLOT(deleteLater()));
  connect(m_socketStreamer, SIGNAL(Finished()), m_socketStreamer, SLOT(deleteLater()));
  connect(inputBuffer.get( ), SIGNAL( WriteFilled( ) ), m_socketStreamer, SLOT( StreamFrame( ) ) );
}

void WebsocketStream::Start(void)
{ 
  m_socketProcessorThread->start();
  m_streamProcessorThread->start();
}

void WebsocketProcessor::Stop(void)
{
  m_running = false;
}

void WebsocketProcessor::ProcessSocket()
{
  //  Tight loop here. The socket will ensure that we wont sit and spin idly
  while(m_running && 0 <= m_socket.yieldTime());
  emit( Finished( ) );
}

void WebsocketStreamer::Init(void)
{
  m_formatConverterImage = shared_ptr<IplImage>(
	cvCreateImage( cvSize( m_inputBuffer->GetWidth(), m_inputBuffer->GetHeight() ), IPL_DEPTH_8U, 3),
	[](IplImage* ptr) { cvReleaseImage( &ptr ); } );
  m_running = true;
}

bool WebsocketStreamer::IsRunning( void )
{
  return m_running;
}

void WebsocketStreamer::Stop(void)
{
  m_running = false;
}

void WebsocketStreamer::StreamFrame(void)
{
  int encodingProperties[] = {CV_IMWRITE_JPEG_QUALITY, 95, 0};
  //int encodingProperties[] = {CV_IMWRITE_PNG_COMPRESSION, 3, 0 };

  ////	Do our image pulling stuff
  auto frame = m_inputBuffer->ReadBuffer();
  cvCvtColor(frame.get(), m_formatConverterImage.get(), CV_RGB2BGR);

  // PROFILE: Hot point in the code (for PNG)
  auto buffer = shared_ptr<CvMat>(
				cvEncodeImage(".jpg", m_formatConverterImage.get(), encodingProperties), 
				[](CvMat* ptr){cvReleaseMat(&ptr);});
  m_socket.broadcastData(buffer->data.ptr, buffer->width);
}