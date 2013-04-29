#	---------------------------------------------------------------------------------------
# 	2010 Nik Karpinsky
#	---------------------------------------------------------------------------------------

#	---------------------------------------------------------------------------------------
#	List all of the sources to be compiled
#	---------------------------------------------------------------------------------------

set(SOURCES	
    Utils.cpp
    MainController.cpp
    MultiOpenGLBuffer.cpp
    OpenGLTripleBuffer.cpp
	ScriptInterface.cpp
    TriMesh.cpp
    SplatField.cpp
    CameraCapture.cpp
    PortalProcessor.cpp
    SixFringeProcessor.cpp
	CalibrationData.cpp
    WebsocketStream.cpp
	)

# Glob our headers together so that they are included in the project
file(GLOB
    HEADERS
    ../include/*.h 
    )	

#   Qt Files
set(MOCS
    ../include/MainController.h
    ../include/IReadBuffer.h
	../include/OpenGLTripleBuffer.h
	../include/MultiOpenGLBuffer.h
    ../include/ScriptInterface.h
    ../include/DynamicallyScriptableQObject.h
    ../include/CameraCapture.h
    ../include/PortalProcessor.h
    ../include/SixFringeProcessor.h
	../include/CalibrationData.h
	../include/WebsocketStream.h)

#   ---------------------------------------------------------------------------------------
#                           Qt Generation
#   ---------------------------------------------------------------------------------------

qt4_wrap_cpp(GENERATED_SOURCES ${MOCS})
source_group("Generated Sources - Do Not Edit" FILES ${GENERATED_SOURCES})

#
#   Add the library
#
add_library("${PROJECT_NAME}" ${SOURCES} ${HEADERS} ${GENERATED_SOURCES} ${MOCS})

#	---------------------------------------------------------------------------------------
#							Copy Shaders to build
#	---------------------------------------------------------------------------------------

add_subdirectory(Shaders)

set(SHADERS_QUALIFIED)
foreach(shader ${SHADER_SOURCES})
	list(APPEND SHADERS_QUALIFIED "Shaders/${shader}")
endforeach()

source_group("Shaders" FILES ${SHADERS_QUALIFIED})
