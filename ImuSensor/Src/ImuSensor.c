/*
 * ImuSensor.c
 *
 *  Created on: Mar 7, 2025
 *      Author: Orsted
 */


#include "../Inc/ImuSensor.h"

ADXL_InitTypeDef IMUSensor;
adxlStatus initSuccess;

void IMU_Init(){
	IMUSensor.IntMode = INT_ACTIVEHIGH;
	IMUSensor.AutoSleep = AUTOSLEEPOFF;
	IMUSensor.Justify = JUSTIFY_SIGNED;
	IMUSensor.LPMode = LPMODE_NORMAL;
	IMUSensor.LinkMode = LINKMODEOFF;
	IMUSensor.Range = RANGE_16G;
	IMUSensor.Rate = BWRATE_400;
	IMUSensor.Resolution = RESOLUTION_10BIT;

	adxlStatus status =  ADXL_Init(&IMUSensor);
	if(status == ADXL_OK){
		ADXL_Measure(ON);
	}
	initSuccess = status;
}

void IMU_Compute(uint16_t* result){
	if(initSuccess != ADXL_OK) return;
	uint16_t accel_buffer[3] = {0};

	ADXL_getAccel(accel_buffer, OUTPUT_SIGNED);

	result[0] = accel_buffer[0];
	result[1] = accel_buffer[1];
	result[2] = accel_buffer[2];
}
