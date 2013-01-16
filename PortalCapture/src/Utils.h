#ifndef _PORTAL_CAPTURE_QGLPBufferGLPaintDevice_H_
#define _PORTAL_CAPTURE_QGLPBufferGLPaintDevice_H_

#include <QGLContext>

#include <QGLPixelBuffer>

class MainContext : public QGLPixelBuffer
{
public:
  MainContext(int width, int height);
  QGLContext* getContext(void);
};

#endif _PORTAL_CAPTURE_QGLPBufferGLPaintDevice_H_