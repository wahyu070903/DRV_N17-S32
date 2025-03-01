/*
 * Encoder.c
 *
 *  Created on: Mar 1, 2025
 *      Author: Orsted
 */

#include "../Inc/Encoder.h"

AS5600_TypeDef Encoder;
extern I2C_HandleTypeDef hi2c1;
static uint8_t init_success = FALSE;

void encoder_init(){
	Encoder.i2cHandle = &hi2c1;
	Encoder.i2cAddr = ENCODER_ADDRESS;
	Encoder.DirPort = ENCODER_PORT;
	Encoder.DirPin = GPIO_PIN_5;

	HAL_StatusTypeDef status = AS5600_Init(&Encoder);
	if(status == HAL_OK) init_success = TRUE;
}

void encoder_getAngle(uint16_t* res){
	uint16_t result = 0;
	HAL_StatusTypeDef status = AS5600_GetRawAngle(&Encoder, &result);
	if(status == HAL_OK){
		*res = result;
	}
}
