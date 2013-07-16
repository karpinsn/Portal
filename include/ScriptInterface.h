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
  QScriptValue						m_global;
  map<QString, shared_ptr<QObject>> m_scriptObjects;

  QScriptEngine m_scriptEngine;
  ConsoleWorker* m_worker;
  QThread* m_workerThread;
  
public:
  ScriptInterface( bool launchConsoleInterface = true );
  
  template <typename T>
	shared_ptr<T> ResolveObject(QString name)
  {
	auto object = m_scriptObjects.at(name);
	Utils::ThrowIfFalse(nullptr != object, "Unable to resolve object" );

	auto requestedObject = dynamic_pointer_cast<T>( object );
	Utils::ThrowIfFalse(nullptr != requestedObject, "Unable to resolve type of object");

	return requestedObject;
  }

  void PushThis( QString thisName );
  void PopThis( void );

public slots:
  void			AddObject( shared_ptr<QObject> object, QString name );
  void			RunScript( QString filename );
  void			RunScript( QObject* obj, QString filename );

public:
  template <typename objectType>
	void		AddObjectType( QString name )
  {
	m_scriptEngine.globalObject( ).setProperty( name, 
	  m_scriptEngine.newFunction( ScriptInterface::CreateScriptableObject<objectType> ) );
  }

  template <typename objectType, typename arg0>
	void		AddObjectType( QString name )
  {
	m_scriptEngine.globalObject( ).setProperty( name, 
	  m_scriptEngine.newFunction( ScriptInterface::CreateScriptableObject<objectType, arg0> ) );
  }

  template <typename objectType, typename arg0, typename arg1>
	void		AddObjectType( QString name )
  {
	m_scriptEngine.globalObject( ).setProperty( name, 
	  m_scriptEngine.newFunction( ScriptInterface::CreateScriptableObject<objectType, arg0, arg1> ) );
  }

  template <typename objectType, typename arg0, typename arg1, typename arg2>
	void		AddObjectType( QString name )
  {
	m_scriptEngine.globalObject( ).setProperty( name, 
	  m_scriptEngine.newFunction( ScriptInterface::CreateScriptableObject<objectType, arg0, arg1, arg2> ) );
  }

  template <typename objectType, typename arg0, typename arg1, typename arg2, typename arg3>
	void		AddObjectType( QString name )
  {
	m_scriptEngine.globalObject( ).setProperty( name, 
	  m_scriptEngine.newFunction( ScriptInterface::CreateScriptableObject<objectType, arg0, arg1, arg2, arg3> ) );
  }

  template <typename objectType>
	void RegisterMetaObjectType( void )
  {
	qScriptRegisterMetaType( &m_scriptEngine, ScriptInterface::Object2ScriptValue<objectType>, ScriptInterface::ScriptValue2Object<objectType> ); 
  }

private:
  template <typename objectType>
	static QScriptValue Object2ScriptValue(QScriptEngine* engine, objectType* const &in)
  { return engine->newQObject( in ); }

  template <typename objectType>
	static void ScriptValue2Object(const QScriptValue &object, objectType* &out)
  { out = qobject_cast<objectType*>( object.toQObject( ) ); }

  template <typename objectType>
	static QScriptValue CreateScriptableObject( QScriptContext* context, QScriptEngine* engine )
  {
	return engine->newQObject( new objectType( ), QScriptEngine::AutoOwnership, QScriptEngine::AutoCreateDynamicProperties );
  }

  template <typename objectType, typename arg0>
	static QScriptValue CreateScriptableObject( QScriptContext* context, QScriptEngine* engine )
  {
	arg0 argument0 = qscriptvalue_cast<arg0>( context->argument(0) );
	
	return engine->newQObject( new objectType( argument0 ), QScriptEngine::AutoOwnership, QScriptEngine::AutoCreateDynamicProperties );
  }

  template <typename objectType, typename arg0, typename arg1>
	static QScriptValue CreateScriptableObject( QScriptContext* context, QScriptEngine* engine )
  {
	arg0 argument0 = qscriptvalue_cast<arg0>( context->argument(0) );
	arg1 argument1 = qscriptvalue_cast<arg1>( context->argument(1) );

	return engine->newQObject( new objectType( argument0, argument1 ), QScriptEngine::AutoOwnership, QScriptEngine::AutoCreateDynamicProperties );
  }

  template <typename objectType, typename arg0, typename arg1, typename arg2>
	static QScriptValue CreateScriptableObject( QScriptContext* context, QScriptEngine* engine )
  {
	arg0 argument0 = qscriptvalue_cast<arg0>( context->argument(0) );
	arg1 argument1 = qscriptvalue_cast<arg1>( context->argument(1) );
	arg2 argument2 = qscriptvalue_cast<arg2>( context->argument(2) );

	return engine->newQObject( new objectType( argument0, argument1, argument2 ), QScriptEngine::AutoOwnership, QScriptEngine::AutoCreateDynamicProperties );
  }

  template <typename objectType, typename arg0, typename arg1, typename arg2, typename arg3>
	static QScriptValue CreateScriptableObject( QScriptContext* context, QScriptEngine* engine )
  {
	arg0 argument0 = qscriptvalue_cast<arg0>( context->argument(0) );
	arg1 argument1 = qscriptvalue_cast<arg1>( context->argument(1) );
	arg2 argument2 = qscriptvalue_cast<arg2>( context->argument(2) );
	arg3 argument3 = qscriptvalue_cast<arg3>( context->argument(3) );

	return engine->newQObject( new objectType( argument0, argument1, argument2, argument3 ), QScriptEngine::AutoOwnership, QScriptEngine::AutoCreateDynamicProperties );
  }

};
#endif	// _PORTAL_CAPTURE_SCRIPT_INTERFACE_H_
