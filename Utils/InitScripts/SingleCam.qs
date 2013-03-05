// Start by making our buffers
Main.NewMultiBuffer("CaptureBuffer", false, true, 2); // 2 since we are using six fringe
Main.NewBuffer("StreamBuffer", true, false);

Main.NewCalibrationData("Capture1Calibration", "Camera1.qs");
Main.NewCaptureContext("Capture", "CaptureBuffer");


Main.AddCaptureBufferToProcess("CaptureBuffer");
Main.InitProcessContext("StreamBuffer");
Main.NewStreamContext("Stream", 7681, "StreamBuffer");

// Once we fill our capture buffer we want to process it
CaptureBuffer.WriteFilled.connect(Process.updateGL);

//  This will call Start() on all of our contexts
Main.Start();
