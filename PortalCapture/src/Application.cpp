#include "Application.h"

int main(int argc, char **argv)
{
	//	Set the logger level
	wrench::Logger::setLogLevel(wrench::LogLevel::Error);

	//	Setup our console application with an event loop
	QCoreApplication app(argc, argv);
	QCoreApplication::setOrganizationName("SL Studios");
	QCoreApplication::setOrganizationDomain("SLStudios.org");
	QCoreApplication::setApplicationName("PortalCapture");

	//	Create and initialize our controller
	MainController controller;
	controller.Init();
	QObject::connect(&controller, SIGNAL( Finished() ), &app, SLOT( quit() ), Qt::QueuedConnection);

	//	This will put start on top of the event loop
	QTimer::singleShot(0, &controller, SLOT( Start() ));

	// Finally start up the event loop
	return app.exec(); 
}
