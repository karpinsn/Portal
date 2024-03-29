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
#include <QMetaType>

#include <memory>
#include <assert.h>
#include <map>

#include <Lens\config-lens.h>
#include <Lens\ICamera.h>
#include <Lens\OpenCVCamera.h>

#ifdef USE_FILE_CAMERA
#include <Lens\FileCamera.h>
#endif

#ifdef USE_POINT_GREY_CAMERA
  #include <Lens\PointGreyCamera.h>
#endif // USE_POINT_GREY_CAMERA

#include "IContext.h"
#include "ITripleBuffer.h"

#include "ScriptInterface.h"
#include "MultiOpenGLBuffer.h"
#include "ISharedGLContextFactory.h"
#include "Utils.h"

#include "CameraCapture.h"
#include "PortalProcessor.h"
#include "SixFringeProcessor.h"
#include "WebsocketStream.h"

using namespace std;

class MainController : public QObject, public ISharedGLContextFactory
{
  Q_OBJECT

private:
  DISALLOW_COPY_AND_ASSIGN(MainController);

  map<QString, shared_ptr<IContext>>  m_contexts;

  shared_ptr<ScriptInterface>		  m_interface;
  shared_ptr<PortalProcessor>		  m_processContext; //  Main Context : Runs on UI thread
  
public:
  MainController( shared_ptr<ScriptInterface> scriptInterface = nullptr );
  void Init(QString initScriptFilename);
  shared_ptr<QGLWidget> MakeSharedContext(void);

public slots:
  void Start(void);
  void Close(void);

signals:
  void Started(void);
  void Finished(void);
};

Q_DECLARE_METATYPE(MainController*)

#endif	// _PORTAL_CAPTURE_MAIN_CONTROLLER_H_
