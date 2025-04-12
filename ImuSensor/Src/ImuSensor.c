/*
 * ImuSensor.c
 *
 *  Created on: Mar 7, 2025
 *      Author: Orsted
 */


#include "../Inc/ImuSensor.h"

MPU6050_t IMUSensor;
extern I2C_HandleTypeDef hi2c2;

void IMU_Init(){
	MPU6050_Init(&hi2c2);
}

void IMU_Compute(double* result){
	MPU6050_Read_All(&hi2c2, &IMUSensor);

	result[0] = IMUSensor.Ax;
	result[1] = IMUSensor.Ay;
	result[2] = IMUSensor.Az;

}
