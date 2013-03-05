// Start by making our buffers
Main.NewMultiBuffer("CaptureBuffer1", false, true, 2); // 2 since we are using six fringe
Main.NewBuffer("StreamBuffer", true, false);

Main.NewCalibrationData("Camera1", "Camera1.qs");
Main.NewCaptureContext("Capture1", "CaptureBuffer1");


Main.AddCaptureBufferToProcess("CaptureBuffer1", "Camera1");
Main.InitProcessContext("StreamBuffer");
Main.NewStreamContext("Stream", 7681, "StreamBuffer");

// Once we fill our capture buffer we want to process it
CaptureBuffer1.WriteFilled.connect(Process.updateGL);

//  This will call Start() on all of our contexts
Main.Start();
