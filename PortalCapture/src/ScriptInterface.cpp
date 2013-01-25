#include "ScriptInterface.h"

void ScriptWorker::AddObject(QObject* object, string name)
{
  QScriptValue value = m_scriptEngine.newQObject(object);
  m_scriptEngine.globalObject().setProperty(name.c_str(), value);
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

void ScriptInterface::AddObject(QObject* object, string name)
{
  m_worker->AddObject(object, name);
}