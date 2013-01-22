#include "WebsocketStream.h"

void WebsocketStream::Init(shared_ptr<OpenGLTripleBuffer> inputBuffer)
{
  //  TODO: Make sure we only call this once

  // Open our socket connection
  m_socket.start(m_port);

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

void WebsocketStreamer::Stop(void)
{
  m_running = false;
}

void WebsocketStreamer::StreamFrame(void)
{
  //int encodingProperties[] = {CV_IMWRITE_JPEG_QUALITY, 90, 0};
  int encodingProperties[] = {CV_IMWRITE_PNG_COMPRESSION, 3, 0 };

  //	Do our image pulling stuff
  auto frame = m_inputBuffer->ReadBuffer();
  //cvCvtColor(frame.get(), frame.get(), CV_RGB2BGR);
	
  auto buffer = shared_ptr<CvMat>(
			  cvEncodeImage(".png", frame.get(), encodingProperties), 
			  [](CvMat* ptr){cvReleaseMat(&ptr);});
  m_socket.broadcastData(buffer->data.ptr, buffer->width);
}