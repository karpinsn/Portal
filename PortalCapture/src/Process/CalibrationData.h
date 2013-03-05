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

#include "../Utils.h"

using namespace std;

class CalibrationData : public QObject, protected QScriptable
{	
  Q_OBJECT
  Q_PROPERTY( QVariantList Distortion WRITE SetDistortion READ GetDistortionAsVariant )
  Q_PROPERTY( QVariantList Extrinsic WRITE SetExtrinsic READ GetExtrinsicAsVariant )
  Q_PROPERTY( QVariantList Intrinsic WRITE SetIntrinsic READ GetIntrinsicAsVariant )

private:
  DISALLOW_COPY_AND_ASSIGN(CalibrationData);

  glm::mat4	  m_extrinsic;
  glm::mat3x4 m_intrinsic;
  float		  m_distortionCoefficients[5];

public:
  CalibrationData(void);

public slots:
  void SetIntrinsic(QVariantList intrinsicMatrixList);
  QVariantList GetIntrinsicAsVariant( );

  void SetDistortion(QVariantList coefficients);
  QVariantList GetDistortionAsVariant( );

  void SetExtrinsic(QVariantList extrinsicMatrixList);
  QVariantList GetExtrinsicAsVariant( );
};

#endif  // _PORTAL_PROCESS_CALIBRATION_DATA_
