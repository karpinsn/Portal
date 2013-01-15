#	---------------------------------------------------------------------------------------
# 	2010 Nik Karpinsky
#	LGPL 2010
#	---------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.6)

#	-----------------------------------------
# 	Set package properties
#	-----------------------------------------
project(PortalCapture)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/../../CMakeModules")
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/${PROJECT_NAME}")

### ---------------------------------------------------------------------------------------
# Configure Dependencies
### ---------------------------------------------------------------------------------------

#	-----------------------------------------
#	QT
#	-----------------------------------------
find_package(Qt4 COMPONENTS QtCore QtOpenGL REQUIRED)
include(${QT_USE_FILE})
set (LIBS ${LIBS} ${QT_LIBRARIES})

#	-----------------------------------------
#	OpenGL
#	-----------------------------------------
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})

#	-----------------------------------------
#	OpenCV
#	-----------------------------------------
find_package(OpenCV REQUIRED)
include_directories(${OPENCV_INCLUDE_DIRS})
set(LIBS ${LIBS} ${OPENCV_LIBRARIES})

#	-----------------------------------------
#	GLEW
#	-----------------------------------------
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
set(LIBS ${LIBS} ${GLEW_LIBRARY})

#	----------------------------------------------------------------
#	Libraries
#	----------------------------------------------------------------
include_directories(../../Libraries)
include_directories(../../Libraries/3rdParty)

#	-----------------------------------------
#	GLM
#	-----------------------------------------
add_subdirectory(../../Libraries/3rdParty/glm/glm ${PROJECT_BINARY_DIR}/Libraries/3rdParty/glm/glm)
include_directories(../../Libraries/3rdParty/glm)
set(LIBS ${LIBS} glm)

#	-----------------------------------------
#	Wrench
#	-----------------------------------------
add_subdirectory(../../Libraries/Wrench/src/wrench ${PROJECT_BINARY_DIR}/Libraries/Wrench)
include_directories(../../Libraries/Wrench/src)
set(LIBS ${LIBS} wrench)

#	-----------------------------------------
#	Antenna
#	-----------------------------------------
add_subdirectory(../../Libraries/Antenna/src/antenna ${PROJECT_BINARY_DIR}/Libraries/Antenna)
include_directories(../../Libraries/Antenna/src)
include_directories(${ANTENNA_INCLUDE_DIRS})
set(LIBS ${LIBS} antenna "${ANTENNA_LIBRARIES}")

#	---------------------------------------------------------------------------------------
#	Generate Documentation
#	---------------------------------------------------------------------------------------
option(SLS_GENERATE_DOC "Build Doxygen Documentation" OFF)
if(SLS_GENERATE_DOC)
	add_subdirectory(../doc ${PROJECT_BINARY_DIR}/doc)
endif(SLS_GENERATE_DOC)

#	---------------------------------------------------------------------------------------
#	List all of the sources to be compiled
#	---------------------------------------------------------------------------------------

set(SOURCES	
	Application.cpp
    Utils.cpp
    MainController.cpp
    Capture/CameraCapture.cpp


    Application.h
    Utils.h
    MainController.h
    Capture/CameraCapture.h
	)	

#   Qt Files
set(MOCS
    MainController.h
    Capture/CameraCapture.h)

#   ---------------------------------------------------------------------------------------
#                           Qt Generation
#   ---------------------------------------------------------------------------------------

qt4_wrap_cpp(GENERATED_SOURCES ${MOCS})

source_group("Generated Sources - Do Not Edit" FILES ${GENERATED_SOURCES})

#	---------------------------------------------------------------------------------------
#							Build the app
#	---------------------------------------------------------------------------------------

# Build and link the app!
add_executable(${PROJECT_NAME}
	MACOSX_BUNDLE
	${SOURCES}
	${SHADERS_QUALIFIED}
    ${MOCS}
    ${GENERATED_SOURCES})

add_dependencies(${PROJECT_NAME} wrench)
add_dependencies(${PROJECT_NAME} antenna)

target_link_libraries(${PROJECT_NAME} ${LIBS})

#	---------------------------------------------------------------------------------------
#							Install
#	---------------------------------------------------------------------------------------

install (TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION bin/
		BUNDLE DESTINATION ./ )

#	---------------------------------------------------------------------------------------
#							Copy Shaders to build
#	---------------------------------------------------------------------------------------

add_subdirectory(Shaders)

set(SHADERS_QUALIFIED)
foreach(shader ${SHADER_SOURCES})
	list(APPEND SHADERS_QUALIFIED "Shaders/${shader}")
endforeach()

source_group("Shaders" FILES ${SHADERS_QUALIFIED})
