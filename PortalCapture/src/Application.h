/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_APPLICATION_H_
#define _PORTAL_CAPTURE_APPLICATION_H_

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

#endif	// _PORTAL_CAPTURE_APPLICATION_H_
