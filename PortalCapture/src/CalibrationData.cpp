#include "CalibrationData.h"

CalibrationData::CalibrationData(void)
{ 
  // Initialize our coefficients to be 0.0 (default)
  for( int coeff = 0; coeff < 5; ++coeff )
  { m_distortionCoefficients[coeff] = 0.0; }

  // Intrinsic and extrinsic will be identity by default constructor
}

void CalibrationData::SetIntrinsic(QVariantList intrinsicMatrixList)
{
  Utils::ThrowIfFalse( 9 == intrinsicMatrixList.count(), "Invalid number of intrinsic matrix coefficients specified" );

  float intrinsic[9];
  for( int valueLoc = 0; valueLoc < intrinsicMatrixList.count(); ++valueLoc )
  {
	Utils::ThrowIfFalse( intrinsicMatrixList[valueLoc].canConvert<float>(), "Invalid intrinsic matrix coefficient type specified" );
	intrinsic[valueLoc] = intrinsicMatrixList[valueLoc].toFloat( );
  }

  // The vector we are reading in is row major, glm is col major, transpose fixes this
  m_intrinsic = glm::transpose(glm::make_mat3(intrinsic));
}

QVariantList CalibrationData::GetIntrinsicAsVariant( void )
{
  auto intrinsics = QVariantList();
  auto flippedIntrinsics = glm::transpose(m_intrinsic);
  for( int valueLoc = 0; valueLoc < 12; ++valueLoc )
  {
	intrinsics.push_back( QVariant( glm::value_ptr( flippedIntrinsics )[valueLoc] ) );
  }

  return intrinsics;
}

const glm::mat3& CalibrationData::GetIntrinsicAsMat( void ) const
{
  return m_intrinsic;
}

void CalibrationData::SetDistortion(QVariantList coefficients)
{
  Utils::ThrowIfFalse( 5 == coefficients.count(), "Invalid number of distortion coefficients specified" );

  for( int coeffNum = 0; coeffNum < coefficients.count(); ++coeffNum )
  {
	Utils::ThrowIfFalse( coefficients[coeffNum].canConvert<float>(), "Invalid distortion coefficient type specified" );
	m_distortionCoefficients[coeffNum] = coefficients[coeffNum].toFloat( );
  }
}

QVariantList CalibrationData::GetDistortionAsVariant( )
{
  auto distortionCoefs = QVariantList();
  for( int valueLoc = 0; valueLoc < 5; ++valueLoc )
  {
	distortionCoefs.push_back( QVariant( m_distortionCoefficients[valueLoc] ) );
  }

  return distortionCoefs;
}

const float* CalibrationData::GetDistortionAsFloatArray( void ) const
{
  return m_distortionCoefficients;
}

void CalibrationData::SetExtrinsic(QVariantList extrinsicMatrixList)
{
  Utils::ThrowIfFalse( 12 == extrinsicMatrixList.count(), "Invalid number of extrinsic matrix coefficients specified" );

  float extrinsic[12];
  for( int valueLoc = 0; valueLoc < extrinsicMatrixList.count(); ++valueLoc )
  {
	Utils::ThrowIfFalse( extrinsicMatrixList[valueLoc].canConvert<float>(), "Invalid extrinsic matrix coefficient type specified" );
	extrinsic[valueLoc] = extrinsicMatrixList[valueLoc].toFloat( );
  }

  // The vector we are reading in is row major, glm is col major, transpose fixes this
  m_extrinsic = glm::transpose(glm::make_mat3x4(extrinsic));
}

QVariantList CalibrationData::GetExtrinsicAsVariant( )
{
  auto extrinsics = QVariantList();
  auto flippedExtrinsics = glm::transpose(m_extrinsic);
  for( int valueLoc = 0; valueLoc < 16; ++valueLoc )
  {
	extrinsics.push_back( QVariant( glm::value_ptr( flippedExtrinsics )[valueLoc] ) );
  }

  return extrinsics;
}

const glm::mat4x3& CalibrationData::GetExtrinsicAsMat( void ) const
{
  return m_extrinsic;
}