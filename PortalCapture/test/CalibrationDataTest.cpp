#include <iostream>

#include "gtest/gtest.h"

#include "CalibrationData.h"

TEST(CalibrationData, CheckCalibrationDataDefault)
{
  CalibrationData data;
  
  // Check that we start as the identity
  EXPECT_EQ(data.GetExtrinsicAsMat(), glm::mat4());
  EXPECT_EQ(data.GetIntrinsicAsMat(), glm::mat4x3());
}