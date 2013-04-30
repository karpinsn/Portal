// Start by making our buffers
Main.NewMultiBuffer("CaptureBuffer1", false, true, 2); // 2 since we are using six fringe
Main.NewMultiBuffer("CaptureBuffer2", false, true, 2); // 2 since we are using six fringe
Main.NewBuffer("StreamBuffer", true, false);

Main.NewCaptureContext("Capture1", "CaptureBuffer1");
Main.NewCaptureContext("Capture2", "CaptureBuffer2");

Main.AddCaptureBufferToProcess("CaptureBuffer1");
Main.AddCaptureBufferToProcess("CaptureBuffer2");

Main.InitProcessContext("StreamBuffer");
Main.NewStreamContext("Stream", 7681, "StreamBuffer");

// Once we fill our capture buffer we want to process it
// Only need to wire up one of the buffers. It will still pull from both
CaptureBuffer2.WriteFilled.connect(Process.updateGL);

//  This will call Start() on all of our contexts
Main.Start();