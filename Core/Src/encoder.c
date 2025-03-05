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
uint16_t position_prev = 0;	// 0 = CCW, 1 = CW
int32_t accumulate_counter = 0;
uint8_t raw_buffer[2] = {0};
uint16_t encoderReadings[FILTER_SIZE];
uint16_t filterIndex = 0;
uint16_t filteredValue = 0;
uint8_t elapsed_fall_f = 0;
uint8_t elapsed_rise_f = 0;
int8_t active_quadrant = -1;
int8_t last_quadrant = -1;

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

void encRead(){
	HAL_I2C_Mem_Read_IT(&hi2c1, (ENC_ADDR << 1), AS5600_RAW_ANGLE_MSB_REG, I2C_MEMADD_SIZE_8BIT, raw_buffer, 2);
}

int32_t getCounter(){
	return accumulate_counter;
}

void encGetBuffer(uint8_t* result){
	result[0] = raw_buffer[0];
	result[1] = raw_buffer[1];
}

uint16_t lowPassFilter(uint16_t newData, uint8_t* finish_f){
	encoderReadings[filterIndex] = newData;
	filterIndex = (filterIndex + 1) % FILTER_SIZE;

	int sum = 0;
	for (int i = 0; i < FILTER_SIZE; i++) {
		sum += encoderReadings[i];
	}

	filteredValue = sum / FILTER_SIZE;

	return filteredValue;
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
    	uint16_t position_now = (raw_buffer[0] << 8) | raw_buffer[1];
//      filter make it worse disable it
//    	position_now = lowPassFilter(position_now);
		active_quadrant = -1;

		if(position_now >= 0 && position_now <= 1024) active_quadrant = 1;
		if(position_now >= 1025 && position_now <= 2048) active_quadrant = 2;
		if(position_now >= 2049 && position_now <= 3072) active_quadrant = 3;
		if(position_now >= 3073 && position_now <= 4095) active_quadrant = 4;

		if(last_quadrant == -1) last_quadrant = active_quadrant;
		int32_t delta = (int32_t)(position_now - position_prev);

		if(last_quadrant == 4 && active_quadrant == 1){
			elapsed_fall_f = TRUE;
			delta = ((ENC_PPR - 1) - abs(delta));
		}else if(last_quadrant == 1 && active_quadrant == 4){
			elapsed_rise_f = TRUE;
			delta = ((ENC_PPR - 1) - delta);
		}

		accumulate_counter += delta;

		elapsed_fall_f = FALSE;
		elapsed_rise_f = FALSE;
		position_prev = position_now;
		last_quadrant = active_quadrant;

    }
}


