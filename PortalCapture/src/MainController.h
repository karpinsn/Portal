/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_MAIN_CONTROLLER_H_
#define _PORTAL_CAPTURE_MAIN_CONTROLLER_H_

//	Needed so that windows.h does not include Winsock.h
#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif
	#include <WinSock2.h>
	#include <windows.h>
#endif

#ifdef __APPLE__
#include <glew.h>
#include <OpenGL/gl.h>
#elif _WIN32
#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#endif

#include <QObject>
#include <QScriptEngine>
#include <QGLWidget>
#include <memory>
#include <assert.h>
#include <map>

#include "IContext.h"
#include "ITripleBuffer.h"

#include "ScriptInterface.h"
#include "MultiOpenGLBuffer.h"
#include "ISharedGLContextFactory.h"
#include "Utils.h"

#include "Capture\CameraCapture.h"
#include "Process\SixFringeProcessor.h"
#include "Stream\WebsocketStream.h"

using namespace std;

class MainController : public QObject, public ISharedGLContextFactory
{
  Q_OBJECT

private:
  DISALLOW_COPY_AND_ASSIGN(MainController);

  map<QString, shared_ptr<IContext>>  m_contexts;

  shared_ptr<ScriptInterface>			m_interface;
  shared_ptr<SixFringeProcessor>  m_processContext; //  Main Context : Runs on UI thread
  
public:
  MainController();
  void Init(QString initScriptFilename);
  shared_ptr<QGLWidget> MakeSharedContext(void);

public slots:
  void Start(void);
  void Close(void);

	// Process Initalizing methods
	void AddCaptureBufferToProcess( QString bufferName );
	void InitProcessContext( QString outputBufferName);

  // Factory methods
  void NewBuffer( QString bufferName, bool makeReadContext, bool makeWriteContext );
  void NewMultiBuffer( QString bufferName, bool makeReadContext, bool makeWriteContext, int bufferCount);
  void NewCaptureContext( QString contextName, QString outputBufferName );
  void NewStreamContext( QString contextName, int port, QString inputBufferName );

private slots:
  void StartSystem(void);

signals:
  void Finished(void);
};

#endif	// _PORTAL_CAPTURE_MAIN_CONTROLLER_H_
