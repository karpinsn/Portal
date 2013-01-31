#include "Application.h"

int main(int argc, char **argv)
{
  // Set the logger level
  wrench::Logger::setLogLevel(wrench::LogLevel::All);

  // Setup our console application with an event loop. Needs to be a QApplication
  // so that we can fake windows for headless running
  // QCoreApplication app(argc, argv);
  QApplication app(argc, argv);
  QApplication::setOrganizationName("SL Studios");
  QApplication::setOrganizationDomain("SLStudios.org");
  QApplication::setApplicationName("PortalCapture");

  // Create and initialize our controller
  MainController controller;
  controller.Init("SingleCam.qs");
  QObject::connect(&controller, SIGNAL( Finished() ), &app, SLOT( quit() ), Qt::QueuedConnection);

  // This will put start on top of the event loop
  //controller.Start();

  // Finally start up the event loop
  return app.exec(); 
}
