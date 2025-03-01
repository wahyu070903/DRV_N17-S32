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

#define ENCODER_ADDRESS 0x36
#define ENCODER_PORT GPIOB
#define ENCODER_DIR GPIO_PIN_5
#define TRUE 1
#define FALSE 0

void encoder_init();
void encoder_getAngle(uint16_t*);
#endif /* INC_ENCODER_H_ */
