// Create our cameras
var cam1 = new PointGreyCamera( );
var cam2 = new PointGreyCamera( );
Global.RunScript( cam1, "RightCameraConfig.js" );
Global.RunScript( cam2, "LeftCameraConfig.js" );

// Load our calibration data
var cam1Config = new CalibrationData( );
var cam2Config = new CalibrationData( );
var projectorConfig = new CalibrationData( );
Global.RunScript( cam1Config, "RightCameraCalibration.js" );
Global.RunScript( cam2Config, "LeftCameraCalibration.js" );
Global.RunScript( projectorConfig, "ProjectorCalibration.js" );

// Buffers for our cameras
var cam1Buffer = new MultiOpenGLBuffer(2, false, true, this); // 2 Since we are using six fringe
var cam2Buffer = new MultiOpenGLBuffer(2, false, true, this); 

// Set the properties on the main process
Process.outputWidth = 512;
Process.outputHeight = 512;
Process.fringeFrequency = 16.0;
Process.pointSize = 3.0;

// Now create capture contexts from our cameras and add them to the process context
var capture1 = new CameraCapture( cam1Buffer, cam1 );
var processor1 = new SixFringeProcessor( cam1Buffer, cam1Config, projectorConfig );
processor1.gammaCutoff = .10;
processor1.intensityCutoff = .10;
processor1.fringePitch1 = 54;
processor1.fringePitch2 = 60;
processor1.Phi0 = -5.1313;
processor1.m = 0.011635528346629;
processor1.b = -1.576614090968211;
Process.AddProcessContext(processor1);

var capture2 = new CameraCapture( cam2Buffer, cam2 );
var processor2 = new SixFringeProcessor( cam2Buffer, cam2Config, projectorConfig );
processor2.gammaCutoff = .10;
processor2.intensityCutoff = .10;
processor2.fringePitch1 = 54;
processor2.fringePitch2 = 60;
processor2.Phi0 = -5.1313;
processor2.m = 0.010300303782262;
processor2.b = -1.560496023012635;
Process.AddProcessContext(processor2);

// Finally, init our process context and its output context
var streamBuffer = new OpenGLTripleBuffer( this, true, false );
Process.Init( streamBuffer );
var streamContext = new WebsocketStream( 7681, streamBuffer );

// Once we fill our capture buffer we want to process it
cam1Buffer.WriteFilled.connect( Process.updateGL );

// Now start up our contexts
streamContext.Start( );
capture1.Start( );
capture2.Start( );