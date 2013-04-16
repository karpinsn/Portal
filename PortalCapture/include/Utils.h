#ifndef _PORTAL_CAPTURE_UTILS_H_
#define _PORTAL_CAPTURE_UTILS_H_

#include <string>
#include <assert.h>

using namespace std;

// Utility Macros:

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

class Utils
{
private:
  static bool m_useAssertions;

public:
  static void AssertOrThrowIfFalse(bool condition, string errorMessage);
};

#endif _PORTAL_CAPTURE_UTILS_H_