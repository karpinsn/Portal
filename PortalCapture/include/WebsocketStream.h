/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_STREAM_WEBSOCKET_STREAM_H_
#define _PORTAL_STREAM_WEBSOCKET_STREAM_H_

//	Needed so that windows.h does not include Winsock.h
#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif
	#include <WinSock2.h>
	#include <windows.h>
#endif

#include <antenna/BaseStation.h>

#include <memory>

#include <QObject>
#include <QThread>

#include <cv.h>
#include <highgui.h>

#include "ITripleBuffer.h"
#include "IContext.h"
#include "Utils.h"
#include "OpenGLTripleBuffer.h"

using namespace std;

class IStreamContext : public QObject, public IContext
{
  Q_OBJECT

public slots:
  virtual void Start( void ) = 0;
};

class WebsocketProcessor : public QObject
{
  Q_OBJECT

  private:
	DISALLOW_COPY_AND_ASSIGN(WebsocketProcessor);

	bool m_running;
	antenna::BaseStation& m_socket;

  public:
	WebsocketProcessor(antenna::BaseStation& socket) : m_socket(socket), m_running(true) { };
	void Stop(void);

  signals:
	void Finished(void);

  public slots:
	void ProcessSocket();
};

class WebsocketStreamer : public QObject
{
  Q_OBJECT

  private:
	DISALLOW_COPY_AND_ASSIGN(WebsocketStreamer);
	bool m_running;
	antenna::BaseStation& m_socket;
	ITripleBuffer* m_inputBuffer;
	shared_ptr<IplImage>	m_formatConverterImage;

  public:
	WebsocketStreamer(antenna::BaseStation& socket, ITripleBuffer* buffer) : m_socket(socket), m_inputBuffer(buffer), m_running(false) { };
	bool IsRunning( void );
	void Stop(void);

  signals:
	void Finished(void);

  public slots:
	void Init(void);
	void StreamFrame(void);
};

class WebsocketStream : public IStreamContext
{
  Q_OBJECT
	
private:
  DISALLOW_COPY_AND_ASSIGN(WebsocketStream);

  // Socket and port that we are connected with
  antenna::BaseStation	m_socket;

  QThread* m_socketProcessorThread;
  QThread* m_streamProcessorThread;

  WebsocketProcessor* m_socketProcessor;
  WebsocketStreamer*  m_socketStreamer;

public:
  WebsocketStream(int port, ITripleBuffer* inputBuffer);

public slots:
  void Start( void );
};

#endif	// _PORTAL_STREAM_WEBSOCKET_STREAM_H_
