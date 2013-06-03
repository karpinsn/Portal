var cam1 = new OpenCVCamera( );
Global.RunScript(cam1, "PointGreyConfig.js");

// Load up the calibration files
var cam1Calibration = new CalibrationData( );
var projectorCalibration = new CalibrationData( );
Global.RunScript( cam1Calibration, "Camera1Calibration.js" );
Global.RunScript( projectorCalibration, "ProjectorCalibration.js" );

//	Create our new camera, its configuration, and its buffer
var cam1Buffer = new MultiOpenGLBuffer( false, false, 2, this);

// Set the properties on the main process
Process.outputWidth = 512;
Process.outputHeight = 512;
Process.fringeFrequency = 16.0;
Process.pointSize = 3.0;

// Now create capture contexts from our cameras and add them to the process context
var capture1 = new CaptureContext( cam1, cam1Buffer );
var processor1 = new NewSixFringeProcessor( cam1Buffer, cam1Calibration, projectorCalibration );
processor1.gammaCutoff = .45;
processor1.intensityCutoff = .1176;
processor1.fringePitch1 = 60;
processor1.fringePitch2 = 63;
processor1.Phi0 = -5.1313;

// Finally, init our process context and its output context
var streamBuffer = new OpenGLTripleBuffer( this, true, false );
Process.InitProcessContext( streamBuffer );
var streamContext = new WebsocketStream( 7681, streamBuffer );

// Once we fill our capture buffer we want to process it
cam1Buffer.WriteFilled.connect(Process.updateGL);

//  This will call Start() on all of our contexts
this.Start();
