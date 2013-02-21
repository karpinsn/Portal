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

#include <map>
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
  map<QString, shared_ptr<QObject>> m_scriptObjects;
  QScriptEngine m_scriptEngine;
  ConsoleWorker* m_worker;
  QThread* m_workerThread;
  
public:
  ScriptInterface( void );
  
  template <typename T>
	shared_ptr<T> ResolveObject(QString name)
  {
	auto object = m_scriptObjects.at(name);
	Utils::AssertOrThrowIfFalse(nullptr != object, "Unable to resolve object" );

	auto requestedObject = dynamic_pointer_cast<T>( object );
	Utils::AssertOrThrowIfFalse(nullptr != requestedObject, "Unable to resolve type of object");

	return requestedObject;
  }

public slots:
  void			AddObject(shared_ptr<QObject> object, QString name);
  void			RunScript(QString filename);
};
#endif	// _PORTAL_CAPTURE_SCRIPT_INTERFACE_H_
