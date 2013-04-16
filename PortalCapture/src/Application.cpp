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

#include <QCoreApplication>
#include <QTimer>

#include <QtGui>

#include "MainController.h"

#include "wrench\Logger.h"

int main(int argc, char **argv)
{
  // Set the logger level
  wrench::Logger::setLogLevel(wrench::Logger::All);

  // Setup our console application with an event loop. Needs to be a QApplication
  // so that we can fake windows for headless running
  // QCoreApplication app(argc, argv);
  QApplication app(argc, argv);
  QApplication::setOrganizationName("SL Studios");
  QApplication::setOrganizationDomain("SLStudios.org");
  QApplication::setApplicationName("PortalCapture");

  // Create and initialize our controller
  auto scriptInterface = make_shared<ScriptInterface>( );
  MainController controller(scriptInterface);
  controller.Init("SingleCam.qs");
  QObject::connect(&controller, SIGNAL( Finished() ), &app, SLOT( quit() ), Qt::QueuedConnection);

  // Finally start up the event loop
  return app.exec(); 
}
