/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_DYNAMICALLY_SCRIPTABLE_QOBJECT_H_
#define _PORTAL_DYNAMICALLY_SCRIPTABLE_QOBJECT_H_

#include <QObject>
#include <QGLWidget>
#include <QScriptable>

class DynamicallyScriptableQGLWidget : public QGLWidget, protected QScriptable
{
  Q_OBJECT
   
public:
  template <typename T> bool SetProperty(const char* name, T property2Set )
  {
	auto variantProperty = QVariant( property2Set );
	return setProperty( name, variantProperty );
  }

  template <typename T> T ResolveProperty(const char* name)
  {
	auto property2Resolve = property(name);
	Utils::ThrowIfFalse(property2Resolve.isValid(), "Invalid property requested");
	
	Utils::ThrowIfFalse(property2Resolve.canConvert<T>(), "Unable to convert property to specified type");
	return property2Resolve.value<T>();
  }
};

class DynamicallyScriptableQObject : public QObject, protected QScriptable
{
  Q_OBJECT
   
public:
  template <typename T> bool SetProperty(const char* name, T property2Set )
  {
	auto variantProperty = QVariant( property2Set );
	return setProperty( name, variantProperty );
  }

  template <typename T> T ResolveProperty(const char* name)
  {
	auto property2Resolve = property(name);
	Utils::ThrowIfFalse(property2Resolve.isValid(), "Invalid property requested");
	
	Utils::ThrowIfFalse(property2Resolve.canConvert<T>(), "Unable to convert property to specified type");
	return property2Resolve.value<T>();
  }
};
#endif	// _PORTAL_DYNAMICALLY_SCRIPTABLE_QOBJECT_H_
