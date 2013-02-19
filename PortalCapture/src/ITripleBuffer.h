/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_I_TRIPLE_BUFFER_H_
#define _PORTAL_CAPTURE_I_TRIPLE_BUFFER_H_

#include "IReadBuffer.h"
#include "IWriteBuffer.h"

class ITripleBuffer : public IWriteBuffer, public IReadBuffer
{
};

#endif // _PORTAL_CAPTURE_I_TRIPLE_BUFFER_H_
