#include "Utils.h"

void Utils::ThrowIfFalse(bool condition, string errorMessage)
{
  if(!condition)
  {
	throw PortalsException(errorMessage);
  }
}