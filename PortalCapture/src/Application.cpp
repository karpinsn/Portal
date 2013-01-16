#include "Application.h"

int main(int argc, char **argv)
{
	//	Set the logger level
	wrench::Logger::setLogLevel(wrench::LogLevel::Error);

	//	Setup our console application with an event loop. Needs to be a QApplication
	//	so that we can fake windows for headless running
	//QCoreApplication app(argc, argv);
	QApplication app(argc, argv);
	QApplication::setOrganizationName("SL Studios");
	QApplication::setOrganizationDomain("SLStudios.org");
	QApplication::setApplicationName("PortalCapture");

	//	Create and initialize our controller
	MainController controller;
	controller.Init();
	QObject::connect(&controller, SIGNAL( Finished() ), &app, SLOT( quit() ), Qt::QueuedConnection);

	//	This will put start on top of the event loop
	QTimer::singleShot(0, &controller, SLOT( Start() ));

	// Finally start up the event loop
	return app.exec(); 
}
