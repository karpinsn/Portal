#include "gtest/gtest.h"

#include <QApplication>

int main(int argc, char **argv)
{
  // Needs to be made so that we can create headless OpenGL stuffs
  QApplication app(argc, argv);

  ::testing::InitGoogleTest(&argc, argv);
  auto testReturn = RUN_ALL_TESTS( );

  system("pause");
  return testReturn;
}