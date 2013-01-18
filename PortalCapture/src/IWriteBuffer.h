/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_I_WRITE_BUFFER_H_
#define _PORTAL_CAPTURE_I_WRITE_BUFFER_H_

class IWriteBuffer
{
public:
  virtual void InitWrite(int width, int height) = 0;
  virtual void Write(const IplImage* data) = 0;
};

#endif // _PORTAL_CAPTURE_I_WRITE_BUFFER_H_
