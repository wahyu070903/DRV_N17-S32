/*
 * encoder.c
 *
 *  Created on: Jan 23, 2025
 *      Author: Orsted
 */


#include "encoder.h"

uint8_t encInit(uint8_t* nodes, uint8_t nodes_size){
	uint8_t i = 0;
	uint8_t available = 0;
	for(i = 0 ; i <= nodes_size ; i++){
		if(nodes[i] == ENC_ADDR){
			available = 1;
			break;
		}
	}
	if(!available){
		return 0;
	}
	return 1;
}

void angleProgram(){
	extern I2C_HandleTypeDef hi2c1;
	uint8_t raw_angle[2] = {0};
	uint8_t confirm[2] = {0};
	HAL_I2C_Mem_Read(&hi2c1, (ENC_ADDR << 1), AS5600_RAW_ANGLE_MSB_REG, I2C_MEMADD_SIZE_8BIT, raw_angle, 2, 100);
	HAL_Delay(10);
	HAL_I2C_Mem_Write(&hi2c1, (ENC_ADDR << 1), AS5600_ZPOS_MSB_REG, I2C_MEMADD_SIZE_8BIT, raw_angle, 2, 100);
	HAL_Delay(10);
	HAL_I2C_Mem_Read(&hi2c1, (ENC_ADDR << 1), AS5600_ZPOS_MSB_REG, I2C_MEMADD_SIZE_8BIT, confirm, 2, 100);
}

void encChangeDir(uint8_t dir){
	/*	1 = forward
			0 = reverse */
	if(dir == 1){
		HAL_GPIO_WritePin(DIR_PORT, DIR_PIN, GPIO_PIN_RESET);
	}else if (dir == 0){
		HAL_GPIO_WritePin(DIR_PORT, DIR_PIN, GPIO_PIN_SET);
	}
}

void encGetAngle(I2C_HandleTypeDef* channel, float* result){
	uint8_t buffer[2] = {0};
	uint16_t angle  = 0;
	HAL_I2C_Mem_Read(channel, (ENC_ADDR << 1), AS5600_RAW_ANGLE_MSB_REG, I2C_MEMADD_SIZE_8BIT, buffer, 2, 100);
	angle = (buffer[0] << 8) | buffer[1];
	float angle_deg = (angle * 360.0) / 4096.0;

	*result = angle_deg;
}

void watchStatus(I2C_HandleTypeDef* channel, uint8_t* M_detected, uint8_t* M_low, uint8_t* M_high, uint8_t* buff){
	uint8_t buffer = 0;
	HAL_I2C_Mem_Read(channel, (ENC_ADDR << 1), AS5600_RAW_ANGLE_MSB_REG, I2C_MEMADD_SIZE_8BIT, &buffer, 1, 100);
	*buff = buffer;
	*M_detected = (buffer & AS5600_MAGNET_DETECT) > 1;
	*M_low = (buffer & AS5600_MAGNET_LOW) > 1;
	*M_high = (buffer & AS5600_MAGNET_HIGH) > 1;
}
