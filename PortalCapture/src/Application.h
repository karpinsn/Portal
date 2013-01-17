/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_APPLICATION_H_
#define _PORTAL_CAPTURE_APPLICATION_H_

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
