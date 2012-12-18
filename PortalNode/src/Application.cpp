#include "Application.h"

int main(int argc, char **argv)
{
	//	Set the logger level
	wrench::Logger::setLogLevel(wrench::LogLevel::Error);

	//	Initalize a context and give it to the window manager
	PortalGLContext context;
	GLUTManager manager(&context, argc, argv);

	// Start up the application run loop
	manager.start();

	return 0; 
}
