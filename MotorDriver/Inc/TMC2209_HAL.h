/*
 * TMC2209_HAL.h
 *
 *  Created on: Feb 19, 2025
 *      Author: Orsted
 */

#ifndef INC_TMC2209_HAL_H_
#define INC_TMC2209_HAL_H_

#pragma once
#include "stm32f1xx_hal.h"
#include <string.h>

#define TMC2209_HAL_PORT GPIOA
#define TMC2209_HAL_PUL GPIO_PIN_0
#define TMC2209_HAL_DIR	GPIO_PIN_1

#pragma pack(push, 1)

typedef union {
	uint32_t value;
	uint8_t data[4];
} TMC2209_payload;

typedef union {
	uint8_t bytes[8];
	struct {
		uint8_t sync;
		uint8_t slave;
		uint8_t address;
		TMC2209_payload payload;
		uint8_t crc;
	} message;
} TMC2209_Write_Datagram_t;

typedef union {
	uint8_t bytes[4];
	struct {
		uint8_t sync;
		uint8_t slave;
		uint8_t address;
		uint8_t crc;
	} message;
} TMC2209_Read_Datagram_t;

#pragma pack(pop)

HAL_StatusTypeDef TMC2209_HAL_Write(uint8_t, uint32_t);
HAL_StatusTypeDef TMC2209_HAL_Read(uint8_t, uint32_t*);

#endif /* INC_TMC2209_HAL_H_ */
