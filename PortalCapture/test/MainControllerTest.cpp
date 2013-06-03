#include <iostream>

#include "gtest/gtest.h"

#include "Utils.h"
#include "MainController.h"

TEST(MainController, CheckNewCamera)
{
  MainController controller( nullptr );
  
  // This should create a new OpenCV camera and not throw an exception
  //EXPECT_NO_THROW( controller.NewCamera("Cam1", "OpenCV", "") );
  //EXPECT_ANY_THROW( controller.NewCamera("Cam2", "Blah", "") );
}