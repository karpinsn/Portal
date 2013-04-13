include(FileCopyTargets)

#	---------------------------------------------------------------------------------------
#	List all of the shader sources to be copied over
#	---------------------------------------------------------------------------------------

set(SHADER_VERT_SOURCES
	PassThrough.vert
)

set(SHADER_FRAG_SOURCES
	MedianFilter3x3.frag
	Fringe2WrappedPhase.frag
	HorizontalMedianFilter.frag
	PhaseFilter.frag
	Wrapped2Unwrapped.frag
	Phase2Depth.frag
    Phase2Coordinate.frag
    Depth2Holo.frag
	RenderTexture.frag
)

set(SHADER_SOURCES ${SHADER_VERT_SOURCES} ${SHADER_FRAG_SOURCES})
set(SHADER_SOURCES "${SHADER_SOURCES}" PARENT_SCOPE)

#	---------------------------------------------------------------------------------------
#	Copy Shaders to build
#	---------------------------------------------------------------------------------------
set(SHADER_OUTPUT "${EXECUTABLE_OUTPUT_PATH}/Shaders")
message(STATUS "Copying shaders to: " ${SHADER_OUTPUT})

add_file_copy_target(MainShaders ${SHADER_OUTPUT} ${SHADER_SOURCES})
add_dependencies(${PROJECT_NAME} MainShaders)
