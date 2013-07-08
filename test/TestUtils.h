#ifndef _PORTAL_UNIT_TEST_TEST_UTILS_H_
#define _PORTAL_UNIT_TEST_TEST_UTILS_H_

#include <fstream>

#include <cv.h>

class TestUtils
{
public:
  static cv::Mat LoadPFM( std::string fileName )
  {
	std::ifstream file;
	file.open(fileName, std::ios::binary | std::ios::in);

	if( !file.is_open( ) )
	  { return cv::Mat( ); } // Return empty mat since it is an invalid file

	char lineBuffer[2048];

	// Header line 1
	file.getline(lineBuffer, 2048);
	int type;
	if (lineBuffer[0] == 'P' && lineBuffer[1] == 'F')
	  { type = CV_32FC3; }
	else if (lineBuffer[0] == 'P' && lineBuffer[1] == 'f')
	  { type = CV_32FC1; }
	else
	  { return cv::Mat(); } // Return empty mat since it is an invalid file

	// Header line 2
	file.getline(lineBuffer, 2048);
	int width, height;
	std::istringstream line(lineBuffer);

	line >> width;
	line >> height;

	if (width <= 0 || height <= 0)
	  { return cv::Mat(); } // Return empty mat since it is an invalid file

	cv::Mat image( height, width, type );

	// Header line 3
	file.getline(lineBuffer, 2048);
	float scale;
	line = std::istringstream(lineBuffer);

	line >> scale;
	scale = abs(scale); // Scale can only be positive

	// Actual file reading
	for (int row = 0; row < height; ++row) 
	{
	  file.read( ( char* )image.ptr( row ), sizeof( float ) * width * image.channels( ) ); 
	}

	// Scale as needed - Can't do this it will convert to uchar
	// cv::scaleAdd
	cv::cvtColor( image, image, CV_RGB2BGR );
	return image;
  } 
};

#endif	// _PORTAL_UNIT_TEST_TEST_UTILS_H_