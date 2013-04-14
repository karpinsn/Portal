#	---------------------------------------------------------------------------------------
# 	2010 Nik Karpinsky
#	---------------------------------------------------------------------------------------

#	---------------------------------------------------------------------------------------
#	List all of the sources to be compiled
#	---------------------------------------------------------------------------------------

set(SOURCES	
	Application.cpp
    Utils.cpp
    MainController.cpp
    MultiOpenGLBuffer.cpp
    OpenGLTripleBuffer.cpp
	ScriptInterface.cpp

    Capture/CameraCapture.cpp
    Process/PortalProcessor.cpp
    Process/SixFringeProcessor.cpp
	Process/CalibrationData.cpp
    Stream/WebsocketStream.cpp
	
    Application.h
    Utils.h
    MainController.h
    OpenGLTripleBuffer.h
    MultiOpenGLBuffer.h
    IReadBuffer.h
    IWriteBuffer.h
    ISharedGLContextFactory.h
	ScriptInterface.h
	IContext.h
    ITripleBuffer.h

    Capture/CameraCapture.h
    Process/PortalProcessor.h
    Process/SixFringeProcessor.h
    Process/CalibrationData.h
	Stream/WebsocketStream.h
    )	

#   Qt Files
set(MOCS
    MainController.h
    IReadBuffer.h
	OpenGLTripleBuffer.h
	MultiOpenGLBuffer.h
    ScriptInterface.h
    Capture/CameraCapture.h
    Process/PortalProcessor.h
    Process/SixFringeProcessor.h
	Process/CalibrationData.h
	Stream/WebsocketStream.h)

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
