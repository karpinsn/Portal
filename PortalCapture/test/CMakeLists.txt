# Enable testing and find GTest
enable_testing()

# Add our tests
set(TESTS
    CalibrationDataTest.cpp
    MainControllerTest.cpp
    ShaderTest.cpp
    )

add_executable(RunUnitTests
    ${TESTS})

# Link against GTest and GText Main
target_link_libraries(RunUnitTests gtest gtest_main ${LIBS})

add_test(NAME RunUnitTests
    COMMAND RunUnitTests)
