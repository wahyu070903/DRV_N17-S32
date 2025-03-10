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
		result = HAL_I2C_IsDeviceReady(channel, (i << 1), 1, 1);
		if(result == HAL_OK){
			found_addr[counter] = i;
			counter++;
		}
		HAL_Delay(5);
	}
}

void i2c_reset(I2C_HandleTypeDef* hi2c) {
	__HAL_RCC_I2C1_FORCE_RESET();
	HAL_Delay(10);
	__HAL_RCC_I2C1_RELEASE_RESET();
	HAL_Delay(10);
    HAL_I2C_DeInit(hi2c);
    HAL_Delay(10);
    HAL_I2C_Init(hi2c);
}

void i2c_bus_recover(I2C_HandleTypeDef* hi2c) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_TypeDef* scl_port;
    GPIO_TypeDef* sda_port;
    uint16_t scl_pin, sda_pin;

    const uint8_t max_timeout = 100;
    static uint8_t timeout_count = 0;

    if (hi2c->Instance == I2C1) {
        scl_port = GPIOB;
        sda_port = GPIOB;
        scl_pin = GPIO_PIN_6;
        sda_pin = GPIO_PIN_7;
    } else if (hi2c->Instance == I2C2) {
        scl_port = GPIOB;
        sda_port = GPIOB;
        scl_pin = GPIO_PIN_10;
        sda_pin = GPIO_PIN_11;
    } else {
        return;
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = scl_pin | sda_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(scl_port, &GPIO_InitStruct);

    while(HAL_GPIO_ReadPin(sda_port, sda_pin) == GPIO_PIN_RESET) {
		HAL_GPIO_WritePin(scl_port, scl_pin, GPIO_PIN_SET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(scl_port, scl_pin, GPIO_PIN_RESET);
		HAL_Delay(1);

		timeout_count ++;
		if(timeout_count > max_timeout) break;
    }

    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    HAL_GPIO_Init(scl_port, &GPIO_InitStruct);
}
