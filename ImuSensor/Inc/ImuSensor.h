/*
 * ImuSensor.h
 *
 *  Created on: Mar 7, 2025
 *      Author: Orsted
 */

#ifndef INC_IMUSENSOR_H_
#define INC_IMUSENSOR_H_

#include "mpu6050.h"

void IMU_Init();
void IMU_Compute(double*);
#endif /* INC_IMUSENSOR_H_ */
