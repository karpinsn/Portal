/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_SCRIPT_INTERFACE_H_
#define _PORTAL_CAPTURE_SCRIPT_INTERFACE_H_

#include <QObject>
#include <QThread>
#include <QScriptEngine>

#include <memory>
#include <iostream>

#include <Wrench\Logger.h>

using namespace std;

class ScriptWorker : public QObject
{
  Q_OBJECT

private:
  QScriptEngine m_scriptEngine;
  bool m_running;

public:
  ScriptWorker( void ) : m_running(true) { };
  void AddObject(QObject* object, string name);

signals:
  void Done( void );

public slots:
  void ProcessInput( void );
};

class ScriptInterface : public QObject
{
  Q_OBJECT

private:
  ScriptWorker* m_worker;
  QThread* m_workerThread;
  
public:
  ScriptInterface( void );
  void AddObject(QObject* object, string name);
};

#endif	// _PORTAL_CAPTURE_SCRIPT_INTERFACE_H_
