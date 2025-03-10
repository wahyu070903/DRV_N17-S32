/*
 * i2c_bus.h
 *
 *  Created on: Jan 23, 2025
 *      Author: Orsted
 */

#ifndef INC_I2C_BUS_H_
#define INC_I2C_BUS_H_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

#define I2C_MAX_NODE 127
#define ENC_1 36
#define I2C_CONNECTED_NODE 2

void i2c_scanbus(I2C_HandleTypeDef*, uint8_t*);
void i2c_reset(I2C_HandleTypeDef*);
void i2c_bus_recover(I2C_HandleTypeDef*);

#endif /* INC_I2C_BUS_H_ */
