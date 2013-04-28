/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_PROCESS_CALIBRATION_DATA_H_
#define _PORTAL_PROCESS_CALIBRATION_DATA_H_

#include <QObject>
#include <QScriptable>
#include <QScriptValue>
#include <QScriptEngine>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "DynamicallyScriptableQObject.h"
#include "Utils.h"

using namespace std;

class CalibrationData : public DynamicallyScriptableQObject
{	
  Q_OBJECT
  Q_PROPERTY( QVariantList Distortion WRITE SetDistortion READ GetDistortionAsVariant )
  Q_PROPERTY( QVariantList Extrinsic WRITE SetExtrinsic READ GetExtrinsicAsVariant )
  Q_PROPERTY( QVariantList Intrinsic WRITE SetIntrinsic READ GetIntrinsicAsVariant )

private:
  DISALLOW_COPY_AND_ASSIGN(CalibrationData);

  glm::mat4x3 m_extrinsic;
  glm::mat3	  m_intrinsic;
  float		  m_distortionCoefficients[5];

public:
  CalibrationData(void);

public slots:
  void SetIntrinsic(QVariantList intrinsicMatrixList);
  QVariantList		  GetIntrinsicAsVariant( void );
  const glm::mat3&	  GetIntrinsicAsMat( void ) const;

  void SetDistortion(QVariantList coefficients);
  QVariantList GetDistortionAsVariant( void );
  const float* GetDistortionAsFloatArray( void ) const;

  void SetExtrinsic(QVariantList extrinsicMatrixList);
  QVariantList		  GetExtrinsicAsVariant( void );
  const glm::mat4x3&  GetExtrinsicAsMat( void ) const;
};

#endif  // _PORTAL_PROCESS_CALIBRATION_DATA_
