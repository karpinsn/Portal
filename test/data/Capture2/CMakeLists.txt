include(FileCopyTargets)

#	---------------------------------------------------------------------------------------
#	List all of the data files to transfer over 
#	---------------------------------------------------------------------------------------

set(TEST_DATA_CAPTURE2_FILES
	LeftCoords.pfm
	RightCoords.pfm
)

#	---------------------------------------------------------------------------------------
#	Copy Shaders to build
#	---------------------------------------------------------------------------------------
set(TEST_DATA_OUTPUT "${EXECUTABLE_OUTPUT_PATH}/data/Capture2")
message(STATUS "Copying test data to: " ${TEST_DATA_OUTPUT})

add_file_copy_target(TestDataCapture2 ${TEST_DATA_OUTPUT} ${TEST_DATA_CAPTURE2_FILES})