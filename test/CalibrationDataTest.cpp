#include <iostream>

#include "gtest/gtest.h"

#include "CalibrationData.h"

TEST(CalibrationData, CheckCalibrationDataDefault)
{
  CalibrationData data;
  
  // Check that we start as the identity
  EXPECT_EQ(data.GetExtrinsicAsMat(), glm::mat4x3());
  EXPECT_EQ(data.GetIntrinsicAsMat(), glm::mat3());
}

TEST(CalibrationData, CheckCalibrationDataVariantSet)
{
  CalibrationData data;

  // GLM is column major, but variant list specifies it row major.
  float extrinsicList[12] = {.1, .2, .3, .4,
							 .5, .6, .7, .8,
							 .9, 1.0, 1.1, 1.2};
  QVariantList extrinsicVariantList; 
  for(int i = 0; i < 12; ++i)
	{ extrinsicVariantList.append(extrinsicList[i]); }

  data.SetExtrinsic(extrinsicVariantList);
  glm::mat4x3 extrinsicMat( glm::transpose( glm::make_mat3x4( extrinsicList ) ) );

  EXPECT_EQ(data.GetExtrinsicAsMat( ), extrinsicMat );
  EXPECT_NE(data.GetExtrinsicAsMat( ), glm::mat4x3( ) );

  // Now check the intrinsic
  float intrinsicList[9] = {.1, .2, .3,
							.4, .5, .6,
							.7, .8, .9};
  QVariantList intrinsicVariantList;
  for(int i = 0; i < 9; ++i)
	{ intrinsicVariantList.append(intrinsicList[i]); }
  data.SetIntrinsic(intrinsicVariantList);
  glm::mat3 intrinsicMat( glm::transpose( glm::make_mat3( intrinsicList ) ) );
  

  EXPECT_EQ(data.GetIntrinsicAsMat( ), intrinsicMat );
  EXPECT_NE(data.GetIntrinsicAsMat( ), glm::mat3( ) );  
}