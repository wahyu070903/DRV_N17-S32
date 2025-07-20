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

static int8_t active_quadrant = -1;
static int8_t last_quadrant = -1;
static int32_t accumulate_counter = 0;
static uint16_t raw_buffer = 0;
static uint16_t position_prev = 0;
static uint8_t elapsed_fall_f = 0;
static uint8_t elapsed_rise_f = 0;

HAL_StatusTypeDef EncoderInit(){
	Encoder.i2cHandle = &hi2c1;
	Encoder.i2cAddr = ENCODER_ADDRESS << 1;
	Encoder.DirPort = ENCODER_PORT;
	Encoder.DirPin = GPIO_PIN_5;

	if(HAL_I2C_IsDeviceReady(Encoder.i2cHandle, Encoder.i2cAddr, 1, 100) != HAL_OK){
//		pushError(&watcherStack, WATCHER_ERR_ENC_LINE);
		return HAL_ERROR;
	}
	AS5600Init_StatusTypedef status = AS5600_Init(&Encoder);
	if(status == AS5600_INIT_OK) init_success = TRUE;
	else{
//		pushError(&watcherStack, WATCHER_ERR_ENC_INIT);
		return HAL_ERROR;
	}

	return HAL_OK;
}

HAL_StatusTypeDef EncoderGetMagAlingment(uint8_t* result){
	uint8_t buffer = 0;
	HAL_StatusTypeDef status = AS5600_GetMagnetStatus(&Encoder, &buffer);
	if(status != HAL_OK) return status;
	result[0] = (buffer >> 3) & 0x01;
	result[1] = (buffer >> 4) & 0x01;
	result[2] = (buffer >> 5) & 0x01;

	return status;
}

void EncoderGetAngle(uint16_t* res){
	uint16_t result = 0;
	HAL_StatusTypeDef status = AS5600_GetRawAngle(&Encoder, &result);
	if(status == HAL_OK){
		*res = result;
	}
}

int32_t EncoderEnablePool(){
	HAL_StatusTypeDef status = AS5600_GetRawAngle(&Encoder, &raw_buffer);
	if(status != HAL_OK){
//		pushError(&watcherStack, WATCHER_ERR_ENC_DISCONNECT);
		return 0;
	}
	active_quadrant = -1;

	if(raw_buffer >= 0 && raw_buffer <= 1024) active_quadrant = 1;
	if(raw_buffer >= 1025 && raw_buffer <= 2048) active_quadrant = 2;
	if(raw_buffer >= 2049 && raw_buffer <= 3072) active_quadrant = 3;
	if(raw_buffer >= 3073 && raw_buffer <= 4095) active_quadrant = 4;

	if(last_quadrant == -1) last_quadrant = active_quadrant;
	int32_t delta = (int32_t)(raw_buffer - position_prev);

	if(delta > 2048) {
		delta -= 4096;
	} else if(delta < -2048) {
		delta += 4096;
	}

	accumulate_counter += delta;

	elapsed_fall_f = FALSE;
	elapsed_rise_f = FALSE;
	position_prev = raw_buffer;
	last_quadrant = active_quadrant;

	return accumulate_counter;
}

