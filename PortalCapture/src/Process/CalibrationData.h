/**
 @file
 @author		Nikolaus Karpinsky
 */

#ifndef _PORTAL_PROCESS_CALIBRATION_DATA_H_
#define _PORTAL_PROCESS_CALIBRATION_DATA_H_

#include <glm/glm.hpp>

#include "../Utils.h"

using namespace std;

class CalibrationData 
{	
private:
  DISALLOW_COPY_AND_ASSIGN(CalibrationData);

  glm::mat4	  m_extrinsic;
  glm::mat3x4 m_intrinsic;
  float		  m_distortionCoefficients[5];

public:
  // TODO - Figure out what we need for constructors.
  // Thinking either explicictly construct the data, or serialize it in
  //CalibrationData(void);
};

#endif  // _PORTAL_PROCESS_CALIBRATION_DATA_
