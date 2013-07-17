include(FileCopyTargets)

#	---------------------------------------------------------------------------------------
#	List all of the data files to transfer over 
#	---------------------------------------------------------------------------------------

set(TEST_DATA_CAPTURE1_FILES
	LeftCameraCalibration.js
	Left1.png
	Left2.png
	LeftComponents.pfm
	LeftPhase.pfm
	LeftPhaseFilt.pfm

	RightCameraCalibration.js
	Right1.png
	Right2.png
	RightComponents.pfm
	RightPhase.pfm

	ProjectorCalibration.js
)

#	---------------------------------------------------------------------------------------
#	Copy Shaders to build
#	---------------------------------------------------------------------------------------
set(TEST_DATA_OUTPUT "${EXECUTABLE_OUTPUT_PATH}/data/Capture1")
message(STATUS "Copying test data to: " ${TEST_DATA_OUTPUT})

add_file_copy_target(TestDataCapture1 ${TEST_DATA_OUTPUT} ${TEST_DATA_CAPTURE1_FILES})