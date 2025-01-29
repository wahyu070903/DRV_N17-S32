/*
 * i2c_bus.c
 *
 *  Created on: Jan 23, 2025
 *      Author: Orsted
 */


#include "i2c_bus.h"

static uint8_t i2c_dataRady = 0;
static uint8_t i2c_sendSuccess = 0;

/* Blocking function */
void i2c_scanbus(I2C_HandleTypeDef* channel, uint8_t* found_addr){
	HAL_StatusTypeDef result;
	static uint8_t counter = 0;
	for(uint8_t i = 0 ; i <= MAX_NODE ; i++){
		result = HAL_I2C_IsDeviceReady(channel, (i << 1), 1, 100);
		if(result == HAL_OK){
			found_addr[counter] = i;
			counter++;
		}
		HAL_Delay(10);
	}
}

void dataReadySet(uint8_t newValue){
	i2c_dataRady = newValue;
}

uint8_t dataReadyGet(){
	return i2c_dataRady;
}

void sendStatusSet(uint8_t newValue){
	i2c_sendSuccess = newValue;
}
uint8_t sendStatusGet(){
	return i2c_sendSuccess;
}
void i2c_readbus_8t(I2C_HandleTypeDef* channel, uint8_t devAddr, uint8_t reg, uint8_t* buffer, uint8_t buff_size){
	HAL_I2C_Mem_Read_IT(channel, devAddr, reg, I2C_MEMADD_SIZE_8BIT, buffer, buff_size);
}

/* memory read callback */
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c){
	if(hi2c->Instance == I2C1)
  {
		dataReadySet(1);
  }
}

/* memory write callback */
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c){
	if(hi2c->Instance == I2C1){
		sendStatusSet(1);
	}
}
