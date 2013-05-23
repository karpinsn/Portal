// Create our cameras
this.NewCamera("Cam1", "PointGrey", "Camera1Config.js");
this.NewCamera("Cam2", "PointGrey", "Camera2Config.js");

// Load our calibration data
this.NewCalibrationData("Cam1Config", "Camera1Calibration.js");
this.NewCalibrationData("Cam2Config", "Camera2Calibration.js");
this.NewCalibrationData("ProjectorConfig", "ProjectorCalibration.js");

// Buffers for our cameras
this.NewMultiBuffer("Cam1Buffer", false, true, 2); // 2 Since we are using six fringe
this.NewMultiBuffer("Cam2Buffer", false, true, 2); 

// Set the properties on the main process
Process.outputWidth = 512;
Process.outputHeight = 512;
Process.fringeFrequency = 16.0;
Process.pointSize = 3.0;

// Now create capture contexts from our cameras and add them to the process context
this.NewCaptureContext("Capture1", "Cam1", "Cam1Buffer");
this.NewSixFringeProcessor("Processor1", "Cam1Buffer", "Cam1Config", "ProjectorConfig");
Processor1.gammaCutoff = .45;
Processor1.intensityCutoff = .1176;
Processor1.fringePitch1 = 60;
Processor1.fringePitch2 = 63;
Processor1.Phi0 = -5.1313;

this.NewCaptureContext("Capture2", "Cam2", "Cam2Buffer");
this.NewSixFringeProcessor("Processor2", "Cam2Buffer", "Cam2Config", "ProjectorConfig");
Processor2.gammaCutoff = .45;
Processor2.intensityCutoff = .1176;
Processor2.fringePitch1 = 60;
Processor2.fringePitch2 = 63;
Processor2.Phi0 = -5.1313;

// Finally, init our process context and its output context
this.NewBuffer("StreamBuffer", true, false);
this.InitProcessContext("StreamBuffer");
this.NewStreamContext("Stream", 7681, "StreamBuffer");

// Once we fill our capture buffer we want to process it
Cam1Buffer.WriteFilled.connect(Process.updateGL);

//  This will call Start() on all of our contexts
this.Start();

