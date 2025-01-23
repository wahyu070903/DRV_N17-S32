/*
 * encoder.h
 *
 *  Created on: Jan 23, 2025
 *      Author: Orsted
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

	#include "stm32f1xx_hal.h"

	#define ENC_ADDR 0x36
	#define DIR_PORT GPIOB
	#define DIR_PIN GPIO_PIN_5

	#define PI 3.14159f
	#define AS5600_ANGLE_MSB_REG 0x0E
	#define AS5600_ANGLE_LSB_REG 0x0F
	#define AS5600_RAW_ANGLE_MSB_REG 0x0C
	#define AS5600_RAW_ANGLE_LSB_REG 0x0D
	#define AS5600_ZPOS_MSB_REG 0x01
	#define AS5600_ZPOS_ANGLE_LSB_REG 0x02
	#define AS5600_STATUS_REG 0x0B
	#define AS5600_MAGNET_HIGH 0x08
	#define AS5600_MAGNET_LOW 0x10
	#define AS5600_MAGNET_DETECT 0x20


	uint8_t endInit(uint8_t*, uint8_t);
	void encGetAngle(I2C_HandleTypeDef*, float*);
	void encGetRawAngle(I2C_HandleTypeDef*, uint8_t*);
	void encChangeDir(uint8_t);
	void writeZpos(I2C_HandleTypeDef*, uint8_t*);
	void angleProgram(void);
	void watchStatus(I2C_HandleTypeDef*, uint8_t*, uint8_t*, uint8_t*, uint8_t*);

#endif /* INC_ENCODER_H_ */
