/*
 * Encoder.h
 *
 *  Created on: Mar 1, 2025
 *      Author: Orsted
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#pragma once
#include "AS5600.h"
#include "watcher.h"

#define ENCODER_ADDRESS 0x36
#define ENCODER_PORT GPIOB
#define ENCODER_DIR GPIO_PIN_5
#define ENC_PPR 4096
#define TRUE 1
#define FALSE 0

HAL_StatusTypeDef EncoderInit();
void EncoderGetAngle(uint16_t*);
HAL_StatusTypeDef EncoderGetMagAlingment(uint8_t*);
int32_t EncoderEnablePool();
#endif /* INC_ENCODER_H_ */
