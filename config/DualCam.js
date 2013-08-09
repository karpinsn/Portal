// Create our cameras
var rightCamera = new PointGreyCamera( );
var leftCamera = new PointGreyCamera( );
Global.RunScript( rightCamera, "RightCameraConfig.js" );
Global.RunScript( leftCamera, "LeftCameraConfig.js" );

// Load our calibration data
var rightCameraConfig = new CalibrationData( );
var leftCameraConfig = new CalibrationData( );
var projectorConfig = new CalibrationData( );
Global.RunScript( rightCameraConfig, "RightCameraCalibration.js" );
Global.RunScript( leftCameraConfig, "LeftCameraCalibration.js" );
Global.RunScript( projectorConfig, "ProjectorCalibration.js" );

// Buffers for our cameras
var rightCameraBuffer = new MultiOpenGLBuffer(2, false, true, this); // 2 Since we are using six fringe
var leftCameraBuffer = new MultiOpenGLBuffer(2, false, true, this); 

// Set the properties on the main process
Process.outputWidth = 512;
Process.outputHeight = 512;
Process.fringeFrequency = 16.0;
Process.pointSize = 3.0;

// Now create capture contexts from our cameras and add them to the process context
var rightCapture = new CameraCapture( rightCameraBuffer, rightCamera );
var rightProcessor = new SixFringeProcessor( rightCameraBuffer, rightCameraConfig, projectorConfig );
rightProcessor.gammaCutoff = .06;
rightProcessor.intensityCutoff = .06;
rightProcessor.fringePitch1 = 96;
rightProcessor.fringePitch2 = 117;
rightProcessor.Phi0 = -3.02523737012350;
rightProcessor.m = 0.011219973762821;
rightProcessor.b = -1.671776090660283;
Process.AddProcessContext(rightProcessor);

var leftCapture = new CameraCapture( leftCameraBuffer, leftCamera );
var leftProcessor = new SixFringeProcessor( leftCameraBuffer, leftCameraConfig, projectorConfig );
leftProcessor.gammaCutoff = .06;
leftProcessor.intensityCutoff = .06;
leftProcessor.fringePitch1 = 96;
leftProcessor.fringePitch2 = 117;
leftProcessor.Phi0 = -3.02523737012350;
leftProcessor.m = 0.011635528346629;
leftProcessor.b = -1.733693723647701;
Process.AddProcessContext(leftProcessor);

// Finally, init our process context and its output context
var streamBuffer = new OpenGLTripleBuffer( this, true, false );
Process.Init( streamBuffer );
var streamContext = new WebsocketStream( 7681, streamBuffer );

// Once we fill our capture buffer we want to process it
rightCameraBuffer.WriteFilled.connect( Process.updateGL );

// Now start up our contexts
streamContext.Start( );
rightCapture.Start( );
leftCapture.Start( );
