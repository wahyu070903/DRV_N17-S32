/*
 * i2c_bus.c
 *
 *  Created on: Jan 23, 2025
 *      Author: Orsted
 */


#include "i2c_bus.h"

/* Blocking function */
void i2c_scanbus(I2C_HandleTypeDef* channel, uint8_t* found_addr){
	HAL_StatusTypeDef result;
	static uint8_t counter = 0;
	for(uint8_t i = 0 ; i <= I2C_MAX_NODE ; i++){
		result = HAL_I2C_IsDeviceReady(channel, (i << 1), 1, 100);
		if(result == HAL_OK){
			found_addr[counter] = i;
			counter++;
		}
		HAL_Delay(10);
	}
}


