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

	#define PI 3.14159f
	#define ENC_PPR 4096
	#define ENC_MAX_COUNT 9999999
	#define ENC_MIN_COUNT -9999999
	#define FILTER_SIZE 1024

	void encChangeDir(uint8_t);
	void encRead(void);
	int32_t getCounter();

#endif /* INC_ENCODER_H_ */
