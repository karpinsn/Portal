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
  Utils::AssertOrThrowIfFalse( 12 == intrinsicMatrixList.count(), "Invalid number of intrinsic matrix coefficients specified" );

  float intrinsic[12];
  for( int valueLoc = 0; valueLoc < intrinsicMatrixList.count(); ++valueLoc )
  {
	Utils::AssertOrThrowIfFalse( intrinsicMatrixList[valueLoc].canConvert<float>(), "Invalid intrinsic matrix coefficient type specified" );
	intrinsic[valueLoc] = intrinsicMatrixList[valueLoc].toFloat( );
  }

  // The vector we are reading in is row major, glm is col major, transpose fixes this
  m_intrinsic = glm::transpose(glm::make_mat4x3(intrinsic));
}

QVariantList CalibrationData::GetIntrinsicAsVariant( )
{
  // TODO: Comeback and fix this
  return QVariantList();
}

void CalibrationData::SetDistortion(QVariantList coefficients)
{
  Utils::AssertOrThrowIfFalse( 5 == coefficients.count(), "Invalid number of distortion coefficients specified" );

  for( int coeffNum = 0; coeffNum < coefficients.count(); ++coeffNum )
  {
	Utils::AssertOrThrowIfFalse( coefficients[coeffNum].canConvert<float>(), "Invalid distortion coefficient type specified" );
	m_distortionCoefficients[coeffNum] = coefficients[coeffNum].toFloat( );
  }
}

QVariantList CalibrationData::GetDistortionAsVariant( )
{
  //  TODO - Fix this
  return QVariantList( );
}

void CalibrationData::SetExtrinsic(QVariantList extrinsicMatrixList)
{
  Utils::AssertOrThrowIfFalse( 16 == extrinsicMatrixList.count(), "Invalid number of extrinsic matrix coefficients specified" );

  float extrinsic[16];
  for( int valueLoc = 0; valueLoc < extrinsicMatrixList.count(); ++valueLoc )
  {
	Utils::AssertOrThrowIfFalse( extrinsicMatrixList[valueLoc].canConvert<float>(), "Invalid extrinsic matrix coefficient type specified" );
	extrinsic[valueLoc] = extrinsicMatrixList[valueLoc].toFloat( );
  }

  // The vector we are reading in is row major, glm is col major, transpose fixes this
  m_extrinsic = glm::transpose(glm::make_mat4x4(extrinsic));
}

QVariantList CalibrationData::GetExtrinsicAsVariant( )
{
  //  TODO - Fix this
  return QVariantList( );
}