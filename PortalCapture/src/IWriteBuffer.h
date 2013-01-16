/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_I_WRITE_BUFFER_H_
#define _PORTAL_CAPTURE_I_WRITE_BUFFER_H_

class IWriteBuffer
{
public:
  virtual void initWrite(int width, int height) = 0;
  virtual void write(void* data, int size) = 0;
};

#endif // _PORTAL_CAPTURE_I_WRITE_BUFFER_H_
