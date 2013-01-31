// Start by making our buffers
Main.AddMultiBuffer("CaptureBuffer", 2); // 2 since we are using six fringe
Main.AddBuffer("StreamBuffer", true, false);

Main.AddCaptureContext("Capture", "CaptureBuffer");
//Main.AddStreamContext("Stream", "StreamBuffer");

//  This will call Start() on all of our contexts
Main.Start();
