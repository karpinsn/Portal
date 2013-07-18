# Enable testing and find GTest
enable_testing()
set(PROJECT_UNIT_TEST_NAME "${PROJECT_NAME}UnitTests")
include(FileCopyTargets)

# Add our tests
set(TESTS
    TestMain.cpp
    CalibrationDataTest.cpp
    MainControllerTest.cpp
    SixFringeProcessorTest.cpp
    FringeProcessorShaderTest.cpp
    PortalProcessorShaderTest.cpp
    PortalProcessorTest.cpp
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
add_dependencies("${PROJECT_UNIT_TEST_NAME}" TestDataCapture1)
add_dependencies("${PROJECT_UNIT_TEST_NAME}" TestDataCapture2)

# Link against GTest and GText Main
target_link_libraries("${PROJECT_UNIT_TEST_NAME}" gtest gtest_main ${LIBS})

add_test(NAME "${PROJECT_UNIT_TEST_NAME}"
    COMMAND "${PROJECT_UNIT_TEST_NAME}")

create_target_launcher("${PROJECT_UNIT_TEST_NAME}"
	WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")
	
set(PROJECT_UNIT_TEST_NAME "${PROJECT_UNIT_TEST_NAME}" PARENT_SCOPE)
