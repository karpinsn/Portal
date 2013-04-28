#include "ScriptInterface.h"

void ConsoleWorker::ProcessInput( void )
{
  while ( m_running )
  {
	string command;
	getline(cin, command);
	m_scriptInterface.m_scriptEngine.evaluate(QString(command.c_str()));

	if( m_scriptInterface.m_scriptEngine.hasUncaughtException( ) )
	{
	  int line = m_scriptInterface.m_scriptEngine.uncaughtExceptionLineNumber();
	  wrench::Logger::logError( "Uncaught exception at line: %d", line );
	}
  }

  emit Done();
}

ScriptInterface::ScriptInterface( void )
{
  // Save the original global object so we can still search for
  // objects, even if the global object is changed.
  m_global = m_scriptEngine.globalObject( );
  
  // Setup our console thread
  m_workerThread = new QThread();
  m_worker = new ConsoleWorker(*this);
  m_worker->moveToThread(m_workerThread);

  connect(m_workerThread, SIGNAL( started( ) ),	  m_worker,		  SLOT( ProcessInput( ) ));
  connect(m_worker,		  SIGNAL( Done( ) ),	  m_workerThread, SLOT( quit( ) ));
  connect(m_workerThread, SIGNAL( finished( ) ),  m_workerThread, SLOT( deleteLater( ) ));
  connect(m_worker,		  SIGNAL( Done( ) ),	  m_worker,		  SLOT( deleteLater( ) ));

  m_workerThread->start();
}

void ScriptInterface::PushThis( QString thisName )
{
  //  Find our object, then set it as 'this' in our new context
  auto thisObject = m_global.property( thisName );
  QScriptContext* context = m_scriptEngine.pushContext( );
  context->setThisObject( thisObject );
}

void ScriptInterface::PopThis( void )
{
  //  Pop the current context and restore the 'this' object
  m_scriptEngine.popContext( );
}

void ScriptInterface::AddObject(shared_ptr<QObject> object, QString name)
{
  m_scriptObjects.insert(make_pair(name, object));
  QScriptValue value = m_scriptEngine.newQObject(object.get(), QScriptEngine::QtOwnership, QScriptEngine::AutoCreateDynamicProperties);
  m_scriptEngine.globalObject().setProperty(name, value);
}

void ScriptInterface::RunScript(QString filename)
{
  QFile scriptFile(filename);
  if ( !scriptFile.open( QIODevice::ReadOnly ) )
  {
	wrench::Logger::logError( "Unable to load script file %s", filename.toLocal8Bit().data() ); 
  }
  QTextStream stream(&scriptFile);
  //  Not the most efficent way but it will work for now
  QString contents = stream.readAll();
  scriptFile.close();

  auto result = m_scriptEngine.evaluate(contents, filename);
  if( m_scriptEngine.hasUncaughtException( ) )
  {
	int line = m_scriptEngine.uncaughtExceptionLineNumber();
	auto exception = m_scriptEngine.uncaughtException();
	wrench::Logger::logError( "Uncaught exception at line: %d %s", line, result.toString().toLocal8Bit().data() );
  }
}