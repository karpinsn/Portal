#
# 2012 Nik Karpinsky
#
###
# Set CMake Stuff
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/../CMakeModules")
include(CreateLaunchers)

#	---------------------------------------------------------------------------
# 	Set package properties
#	---------------------------------------------------------------------------
project(PortalCapture)
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/${PROJECT_NAME}")

#   ---------------------------------------------------------------------------
# Configure Dependencies
#   ---------------------------------------------------------------------------

#	QT
#	-----------------------------------------
find_package(Qt4 COMPONENTS QtCore QtGui QtOpenGL QtScript REQUIRED)
include(${QT_USE_FILE})
set (LIBS ${LIBS} ${QT_LIBRARIES})

#	OpenGL
#	-----------------------------------------
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})

#	OpenCV
#	-----------------------------------------
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
set(LIBS ${LIBS} ${OpenCV_LIBS})

#	GLEW
#	-----------------------------------------
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
set(LIBS ${LIBS} ${GLEW_LIBRARY})

#	Libraries
#	----------------------------------------------------------------
include_directories(../Libraries)
include_directories(../Libraries/3rdParty)

#	GLM
#	-----------------------------------------
add_subdirectory(../Libraries/3rdParty/glm/glm ${PROJECT_BINARY_DIR}/Libraries/3rdParty/glm/glm)
include_directories(../Libraries/3rdParty/glm)
set(LIBS ${LIBS} glm)

#	Wrench
#	-----------------------------------------
add_subdirectory(../Libraries/Wrench/src/wrench ${PROJECT_BINARY_DIR}/Libraries/Wrench)
include_directories(../Libraries/Wrench/src)
set(LIBS ${LIBS} wrench)

#	Antenna
#	-----------------------------------------
add_subdirectory(../Libraries/Antenna/src/antenna ${PROJECT_BINARY_DIR}/Libraries/Antenna)
include_directories(../Libraries/Antenna/src)
include_directories(${ANTENNA_INCLUDE_DIRS})
set(LIBS ${LIBS} antenna "${ANTENNA_LIBRARIES}")

#	Lens
#	-----------------------------------------
add_subdirectory(../Libraries/Lens/src/lens ${PROJECT_BINARY_DIR}/Libraries/Lens)
include_directories(../Libraries/Lens/src)
include_directories(${LENS_INCLUDE_DIRS})
set(LIBS ${LIBS} lens "${LENS_LIBRARIES}")

#	---------------------------------------------------------------------------
#	Generate Documentation
#	---------------------------------------------------------------------------
option(PortalCapture_GENERATE_DOC "Build Doxygen Documentation" OFF)
if(PortalCapture_GENERATE_DOC)
	add_subdirectory(doc ${PROJECT_BINARY_DIR}/doc)
endif(PortalCapture_GENERATE_DOC)

#   ---------------------------------------------------------------------------
#   Add the source
#   ---------------------------------------------------------------------------
include_directories("include")
add_subdirectory(src)

set(LIBS ${LIBS} "${PROJECT_NAME}_LIB")

#   ---------------------------------------------------------------------------
#   Add Unit tests
#   ---------------------------------------------------------------------------
option(PortalCapture_UNIT_TESTS "Generate PortalCapture unit tests" OFF)
if(PortalCapture_UNIT_TESTS)
    # Fixes a bug with VS 2012 compiler and varadic templates
    add_definitions(-D_VARIADIC_MAX=10)

    add_subdirectory(../Libraries/GTest "${PROJECT_BINARY_DIR}/src/GTest")
    include_directories("${gtest_SOURCE_DIR}/include" "${gtest_SOURCE_DIR}") 
    add_subdirectory(test)
endif()

#   ---------------------------------------------------------------------------
#   Build and link the app!
#   ---------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
	MACOSX_BUNDLE
    src/Application.cpp    
	${SHADERS_QUALIFIED})

set(LIBS ${LIBS} "${PROJECT_NAME}_LIB")

add_dependencies(${PROJECT_NAME} wrench)
add_dependencies(${PROJECT_NAME} antenna)
add_dependencies(${PROJECT_NAME} "${PROJECT_NAME}_LIB")
add_dependencies(${PROJECT_NAME} Shaders)

target_link_libraries(${PROJECT_NAME} ${LIBS})

#	---------------------------------------------------------------------------
#	Install
#	---------------------------------------------------------------------------

install (TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION bin/
		BUNDLE DESTINATION ./ )
		
create_target_launcher(${PROJECT_NAME}
	WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")
		