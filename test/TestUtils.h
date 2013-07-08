#ifndef _PORTAL_UNIT_TEST_TEST_UTILS_H_
#define _PORTAL_UNIT_TEST_TEST_UTILS_H_

#include <fstream>

#include <cv.h>

class TestUtils
{
public:
  static bool WritePFM( std::string fileName, cv::Mat image )
  {
	std::ofstream file;
	file.open( fileName, std::ios::binary | std::ios::out );

	if( !file.is_open( ) )
	  { return false; } // Unable to save the file, return false for failure

	// Header line 1
	// Modified for 4 channel images. This is not part of the standard
	file << 'P';
	if( 4 == image.channels() )
	{
	  file << 'F';
	  file << 'A';
	}
	else if ( 3 == image.channels( ) )
	{
	  file << 'F';
	}
	else if ( 1 == image.channels( ) )
	{
	  file << 'f';
	} // TODO - Need to check for other channel counts and error approprately
	file << '\n';

	// Header line 2
	auto size = image.size; // Remember that matricies are column major
	std::ostringstream line;
	file << size[1]; // Width
	file << ' ';
	file << size[0]; // Height
	file << '\n';

	// Header line 3
	file << 1.0f; // Scale
	file << '\n';

	// Convert to RGB if needed 
	// Convert to OpenCV channel sequence
	switch( image.type( ) )
	{
	case CV_32FC3:
	  cv::cvtColor( image, image, CV_RGB2BGR );
	  break;
	case CV_32FC4:
	  cv::cvtColor( image, image, CV_RGBA2BGRA );
	  break;
	default:
	  break;
	}

	// Now actually write out the float values
	for( int row = 0; row < size[0]; ++row )
	{
	  // Remember that images are not guarentteed to be contingious.
	  // Need to write a row at a time
	  file.write( ( char* )image.ptr( row ), sizeof( float ) * size[1] * image.channels( ) );
	}
	file.close( );

	return true;
  }

  static cv::Mat LoadPFM( std::string fileName )
  {
	std::ifstream file;
	file.open(fileName, std::ios::binary | std::ios::in);

	if( !file.is_open( ) )
	  { return cv::Mat( ); } // Return empty mat since it is an invalid file

	char lineBuffer[2048];

	// Header line 1
	// Modified for 4 channel images. This is not part of the standard
	file.getline(lineBuffer, 2048);
	int type;
	if( lineBuffer[0] == 'P' && lineBuffer[1] == 'F' && lineBuffer[2] == 'A' )
	  { type = CV_32FC4; }
	else if (lineBuffer[0] == 'P' && lineBuffer[1] == 'F')
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
	file.close( );

	// Scale as needed - Can't do this it will convert to uchar
	// cv::scaleAdd
	
	// Convert to OpenCV channel sequence
	switch( type )
	{
	case CV_32FC3:
	  cv::cvtColor( image, image, CV_RGB2BGR );
	  break;
	case CV_32FC4:
	  cv::cvtColor( image, image, CV_RGBA2BGRA );
	  break;
	default:
	  break;
	}

	return image;
  } 
};

#endif	// _PORTAL_UNIT_TEST_TEST_UTILS_H_