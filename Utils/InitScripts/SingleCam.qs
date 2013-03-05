// Start by making our buffers
this.NewMultiBuffer("CaptureBuffer1", false, true, 2); // 2 since we are using six fringe
this.NewBuffer("StreamBuffer", true, false);

this.NewCalibrationData("Camera1", "Camera1.qs");
this.NewCaptureContext("Capture1", "CaptureBuffer1");
this.NewSixFringeProcessor("Processor1", "CaptureBuffer1", "Camera1");

this.InitProcessContext("StreamBuffer");
this.NewStreamContext("Stream", 7681, "StreamBuffer");

// Once we fill our capture buffer we want to process it
CaptureBuffer1.WriteFilled.connect(Process.updateGL);

//  This will call Start() on all of our contexts
this.Start();
