//	Create our new camera, its configuration, and its buffer
this.NewCamera("Cam1", "PointGrey", "PointGreyConfig.js");
//this.NewCamera("Cam1", "FileCamera", "");
//Cam1.open();
this.NewCalibrationData("Cam1Config", "Camera1Calibration.js");
this.NewCalibrationData("ProjectorConfig", "ProjectorCalibration.js");
this.NewMultiBuffer("Cam1Buffer", false, true, 2); // 2 since we are using six fringe

// Now create capture contexts from our cameras and add them to the process context
this.NewCaptureContext("Capture1", "Cam1", "Cam1Buffer");
Capture1.fringePitch1 = 60;
Capture1.fringePitch2 = 63;
Capture1.Phi0 = -5.1313;

this.NewSixFringeProcessor("Processor1", "Cam1Buffer", "Cam1Config", "ProjectorConfig");

// Finally, init our process context and its output context
this.NewBuffer("StreamBuffer", true, false);
this.InitProcessContext("StreamBuffer");
this.NewStreamContext("Stream", 7681, "StreamBuffer");

// Once we fill our capture buffer we want to process it
Cam1Buffer.WriteFilled.connect(Process.updateGL);

//  This will call Start() on all of our contexts
this.Start();
