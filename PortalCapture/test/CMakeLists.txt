# Enable testing and find GTest
enable_testing()

set(PROJECT_UNIT_TEST_NAME "${PROJECT_NAME}UnitTests"
    )

# Add our tests
set(TESTS
    CalibrationDataTest.cpp
    MainControllerTest.cpp
    ShaderTest.cpp
    SixFringeProcessorTest.cpp
    )

add_executable("${PROJECT_UNIT_TEST_NAME}"
    ${TESTS})

# Link against GTest and GText Main
target_link_libraries("${PROJECT_UNIT_TEST_NAME}" gtest gtest_main ${LIBS})

add_test(NAME "${PROJECT_UNIT_TEST_NAME}"
    COMMAND "${PROJECT_UNIT_TEST_NAME}")

# Make sure the parent script can find us so that it can create launchers
set(PROJECT_UNIT_TEST_NAME "${PROJECT_UNIT_TEST_NAME}" PARENT_SCOPE)
