/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_CAPTURE_SCRIPT_INTERFACE_H_
#define _PORTAL_CAPTURE_SCRIPT_INTERFACE_H_

#include <QObject>
#include <QThread>
#include <QScriptEngine>
#include <QFile>
#include <QTextStream>

#include <memory>
#include <iostream>

#include <Wrench\Logger.h>

#include "Utils.h"

using namespace std;

class ScriptInterface;
class ConsoleWorker : public QObject
{
  Q_OBJECT

private:
  ScriptInterface& m_scriptInterface;
  bool m_running;

public:
  ConsoleWorker( ScriptInterface& scriptInterface ) : m_running(true), m_scriptInterface(scriptInterface) { };

signals:
  void Done( void );

public slots:
  void ProcessInput( void );
};

class ScriptInterface : public QObject
{
  Q_OBJECT
  friend ConsoleWorker;

private:
  QScriptEngine m_scriptEngine;
  ConsoleWorker* m_worker;
  QThread* m_workerThread;
  
public:
  ScriptInterface( void );

public slots:
  void AddObject(QObject* object, QString name);
  void RunScript(QString filename);
};

#endif	// _PORTAL_CAPTURE_SCRIPT_INTERFACE_H_
