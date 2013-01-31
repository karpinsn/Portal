/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_I_TRIPLE_BUFFER_H_
#define _PORTAL_CAPTURE_I_TRIPLE_BUFFER_H_

#include "IOpenGLReadBuffer.h"
#include "IWriteBuffer.h"

class ITripleBuffer : public IOpenGLWriteBuffer, public IOpenGLReadBuffer
{
};

#endif // _PORTAL_CAPTURE_I_TRIPLE_BUFFER_H_
