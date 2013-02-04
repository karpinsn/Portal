// Start by making our buffers
Main.AddMultiBuffer("CaptureBuffer", false, true, 2); // 2 since we are using six fringe
Main.AddBuffer("StreamBuffer", true, false);

Main.AddCaptureContext("Capture", "CaptureBuffer");
Main.InitProcessContext("CaptureBuffer", "StreamBuffer");
Main.AddStreamContext("Stream", 7681, "StreamBuffer");

// Once we fill our capture buffer we want to process it
CaptureBuffer.WriteFilled.connect(Process.updateGL);

//  This will call Start() on all of our contexts
Main.Start();
