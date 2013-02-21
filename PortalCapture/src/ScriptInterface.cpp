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
  m_workerThread = new QThread();
  m_worker = new ConsoleWorker(*this);
  m_worker->moveToThread(m_workerThread);

  connect(m_workerThread, SIGNAL( started( ) ),	  m_worker,		  SLOT( ProcessInput( ) ));
  connect(m_worker,		  SIGNAL( Done( ) ),	  m_workerThread, SLOT( quit( ) ));
  connect(m_workerThread, SIGNAL( finished( ) ),  m_workerThread, SLOT( deleteLater( ) ));
  connect(m_worker,		  SIGNAL( Done( ) ),	  m_worker,		  SLOT( deleteLater( ) ));

  m_workerThread->start();
}

void ScriptInterface::AddObject(shared_ptr<QObject> object, QString name)
{
  m_scriptObjects.insert(make_pair(name, object));
  QScriptValue value = m_scriptEngine.newQObject(object.get());
  m_scriptEngine.globalObject().setProperty(name, value);
}

void ScriptInterface::RunScript(QString filename)
{
  // TODO: Could have problems if we are already processing input
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