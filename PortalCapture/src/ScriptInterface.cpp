#include "ScriptInterface.h"

void ScriptWorker::AddObject(QObject* object, QString name)
{
  QScriptValue value = m_scriptEngine.newQObject(object);
  m_scriptEngine.globalObject().setProperty(name, value);
}

void ScriptWorker::ProcessInput( void )
{
  while ( m_running )
  {
	string command;
	getline(cin, command);
	m_scriptEngine.evaluate(QString(command.c_str()));

	if( m_scriptEngine.hasUncaughtException( ) )
	{
	  int line = m_scriptEngine.uncaughtExceptionLineNumber();
	  wrench::Logger::logError( "Uncaught exception at line: %d", line );
	}
  }

  emit Done();
}

void ScriptWorker::RunScript( QString filename )
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

  m_scriptEngine.evaluate(contents, filename);
  if( m_scriptEngine.hasUncaughtException( ) )
  {
	int line = m_scriptEngine.uncaughtExceptionLineNumber();
	wrench::Logger::logError( "Uncaught exception at line: %d", line );
  }
}

ScriptInterface::ScriptInterface( void )
{
  m_workerThread = new QThread();
  m_worker = new ScriptWorker();
  m_worker->moveToThread(m_workerThread);

  connect(m_workerThread, SIGNAL( started( ) ),	  m_worker,		  SLOT( ProcessInput( ) ));
  connect(m_worker,		  SIGNAL( Done( ) ),	  m_workerThread, SLOT( quit( ) ));
  connect(m_workerThread, SIGNAL( finished( ) ),  m_workerThread, SLOT( deleteLater( ) ));
  connect(m_worker,		  SIGNAL( Done( ) ),	  m_worker,		  SLOT( deleteLater( ) ));

  m_workerThread->start();
}

void ScriptInterface::AddObject(QObject* object, QString name)
{
  m_worker->AddObject(object, name);
}

void ScriptInterface::RunScript(QString filename)
{
  // TODO: Could have problems if we are already processing input
  m_worker->RunScript(filename);
}