/*
 * encoder.c
 *
 *  Created on: Jan 23, 2025
 *      Author: Orsted
 */


#include "encoder.h"
#include <stdlib.h>

extern I2C_HandleTypeDef hi2c1;
uint8_t rotation_direction = 0;
int32_t counter = 0;
uint16_t position_prev = 0;	// 0 = CCW, 1 = CW

void encChangeDir(uint8_t direction){
	if(direction == 1){
		//Clockwise
		HAL_GPIO_WritePin(DIR_PORT, DIR_PIN, GPIO_PIN_SET);
		rotation_direction = 1;
	}else if (direction == 0){
		//Counter clockwise
		HAL_GPIO_WritePin(DIR_PORT, DIR_PIN, GPIO_PIN_RESET);
		rotation_direction = 0;
	}
}

void encGetAngle(float* result){
	uint8_t buffer[2] = {0};
	uint16_t angle  = 0;
	HAL_I2C_Mem_Read(&hi2c1, (ENC_ADDR << 1), AS5600_RAW_ANGLE_MSB_REG, I2C_MEMADD_SIZE_8BIT, buffer, 2, 100);
	angle = (buffer[0] << 8) | buffer[1];
	float angle_deg = (angle * 360.0) / 4096.0;

	*result = angle_deg;
}

void encGetCount(int32_t* result){
	HAL_StatusTypeDef status;
	uint8_t buffer[2] = {0};
	uint16_t position_now = 0;
	int32_t delta = 0;
	status = HAL_I2C_Mem_Read(&hi2c1, (ENC_ADDR << 1), AS5600_RAW_ANGLE_MSB_REG, I2C_MEMADD_SIZE_8BIT, buffer, 2, 100);

	if(status == HAL_OK){
		position_now = (buffer[0] << 8) | buffer[1];
		delta = (int32_t)(position_now - position_prev);

		//if(abs(delta) < ENC_NOISE_THRESHOLD) delta = 0;

		if(delta > ENC_PPR / 2){
			delta -= ENC_PPR;
		}else if(delta < -ENC_PPR / 2){
			delta += ENC_PPR;
		}

		if(rotation_direction == 0) delta = -delta;

		counter += delta;

		if(counter >= ENC_MAX_COUNT){
			counter -= ENC_MAX_COUNT;
		}else if(counter < ENC_MIN_COUNT){
			counter += ENC_MAX_COUNT;
		}

		position_prev = position_now;
		*result = counter;
	}
}

