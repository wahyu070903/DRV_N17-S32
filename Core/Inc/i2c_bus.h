/*
 * i2c_bus.h
 *
 *  Created on: Jan 23, 2025
 *      Author: Orsted
 */

#ifndef INC_I2C_BUS_H_
#define INC_I2C_BUS_H_

	#include "stm32f1xx_hal.h"

	#define MAX_NODE 127
	#define ENC_1 36
	#define MAX_QUEUE_SIZE 50

	void i2c_scanbus(I2C_HandleTypeDef*, uint8_t*);

#endif /* INC_I2C_BUS_H_ */
