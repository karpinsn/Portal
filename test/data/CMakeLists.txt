include(FileCopyTargets)

#	---------------------------------------------------------------------------------------
#	List all of the data files to transfer over 
#	---------------------------------------------------------------------------------------

set(TEST_DATA_FILES
	TestHolo1.png
	UnwrappedPhase-30-39.pfm
	WrappedPhaseComponents-30-39.pfm
)

#	---------------------------------------------------------------------------------------
#	Copy Shaders to build
#	---------------------------------------------------------------------------------------
set(TEST_DATA_OUTPUT "${EXECUTABLE_OUTPUT_PATH}/data")
message(STATUS "Copying test data to: " ${TEST_DATA_OUTPUT})

add_file_copy_target(TestData ${TEST_DATA_OUTPUT} ${TEST_DATA_FILES})
