#include "Application.h"

int main(int argc, char **argv)
{
	//	Set the logger level
	wrench::Logger::setLogLevel(wrench::LogLevel::Error);

	// Start up the application run loop
	return 0; 
}
