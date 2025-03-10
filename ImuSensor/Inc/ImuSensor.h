/*
 * ImuSensor.h
 *
 *  Created on: Mar 7, 2025
 *      Author: Orsted
 */

#ifndef INC_IMUSENSOR_H_
#define INC_IMUSENSOR_H_

#include "ADXL345.h"

void IMU_Init();
void IMU_Compute(uint16_t*);
#endif /* INC_IMUSENSOR_H_ */
