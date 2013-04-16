#include "Utils.h"

void Utils::AssertOrThrowIfFalse(bool condition, string errorMessage)
{
  if(!condition)
  {
	//	We failed our condition. Try assert so that we have stack trace
	//	otherwise throw an exception
	// TODO - Think about whether or not this should be here. It makes unit testing a pain
	//assert(condition);
	throw errorMessage;
  }
}