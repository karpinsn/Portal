# Enable testing and find GTest
enable_testing()
set(PROJECT_UNIT_TEST_NAME "${PROJECT_NAME}UnitTests")

# Add our tests
set(TESTS
    CalibrationDataTest.cpp
    MainControllerTest.cpp
    ShaderTest.cpp
    SixFringeProcessorTest.cpp
    TestUtils.h
    )

#
# Grab any test data
#
add_subdirectory(data)

#
# Make our test executable
#
add_executable("${PROJECT_UNIT_TEST_NAME}"
    ${TESTS})

add_dependencies("${PROJECT_UNIT_TEST_NAME}" TestData)

# Link against GTest and GText Main
target_link_libraries("${PROJECT_UNIT_TEST_NAME}" gtest gtest_main ${LIBS})

add_test(NAME "${PROJECT_UNIT_TEST_NAME}"
    COMMAND "${PROJECT_UNIT_TEST_NAME}")

create_target_launcher("${PROJECT_UNIT_TEST_NAME}"
	WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")
	
set(PROJECT_UNIT_TEST_NAME "${PROJECT_UNIT_TEST_NAME}" PARENT_SCOPE)
