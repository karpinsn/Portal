#include "MainController.h"

MainController::MainController() : QObject()
{

}

void MainController::Init(void)
{
  //QGLPixelBuffer* buffer = new QGLPixelBuffer(800, 600);
  MainContext* buffer = new MainContext(800, 600);
  bool has		= buffer->hasOpenGLPbuffers();
  bool current	= buffer->makeCurrent();
  bool valid	= buffer->isValid();

  //  Assuming that since our QGLPixelBuffer created and made the context current
  //  that this will be created on that context. 
  QGLFramebufferObject object(800, 600);  

  object.bind();
  {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	QImage image1 = object.toImage();
	image1.save("Out1.jpg");

	glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	QImage image2 = object.toImage();
	image2.save("Out2.jpg");
  }

  //  Now I need to get access to the context to share it with additional threads

  m_captureContext = unique_ptr<ICaptureContext>( new CameraCapture() );
  m_captureContext->Init();

  
}

void MainController::Start(void)
{
  m_captureContext->Start();
}

void MainController::Close(void)
{
  //  This will tell the event loop that we are done and close the app
  emit( Finished() );
}