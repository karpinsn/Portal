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
Process.angle = 30.0;
Process.pointSize = 3.0;
Process.scale = 1.0/225.0;
Process.xShift = 0.0;
Process.yShift = 150.0;
Process.zShift = -850.0;

// Now create capture contexts from our cameras and add them to the process context
var rightCapture = new CameraCapture( rightCameraBuffer, rightCamera );
var rightProcessor = new SixFringeProcessor( rightCameraBuffer, rightCameraConfig, projectorConfig );
rightProcessor.gammaCutoff = .06;
rightProcessor.intensityCutoff = .06;
rightProcessor.fringePitch1 = 57;
rightProcessor.fringePitch2 = 69;
rightProcessor.Phi0 = -3.02523737012350;
rightProcessor.m = 0.010685689297924;
rightProcessor.b = -1.325025472942634;
Process.AddProcessContext(rightProcessor);

var leftCapture = new CameraCapture( leftCameraBuffer, leftCamera );
var leftProcessor = new SixFringeProcessor( leftCameraBuffer, leftCameraConfig, projectorConfig );
leftProcessor.gammaCutoff = .06;
leftProcessor.intensityCutoff = .06;
leftProcessor.fringePitch1 = 57;
leftProcessor.fringePitch2 = 69;
leftProcessor.Phi0 = -3.02523737012350;
leftProcessor.m = 0.010649466622338;
leftProcessor.b = -0.521823864494576;
Process.AddProcessContext(leftProcessor);

// Finally, init our process context and its output context
// Use 4 channel so that we can bring a texture in the alpha channel
var streamBuffer = new OpenGLTripleBuffer( this, true, false, 4 );
Process.Init( streamBuffer );
var streamContext = new WebsocketStream( 7681, streamBuffer );

// Once we fill our capture buffer we want to process it
rightCameraBuffer.WriteFilled.connect( Process.updateGL );

// Now start up our contexts
streamContext.Start( );
rightCapture.Start( );
leftCapture.Start( );
