/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_I_SHARED_GL_CONTEXT_FACTORY_H_
#define _PORTAL_CAPTURE_I_SHARED_GL_CONTEXT_FACTORY_H_

#include <memory>

#include <QGLWidget>

using namespace std;

class ISharedGLContextFactory
{
public:
  virtual shared_ptr<QGLContext> MakeSharedContext(void) = 0;
};

#endif //_PORTAL_CAPTURE_I_SHARED_GL_CONTEXT_FACTORY_H_