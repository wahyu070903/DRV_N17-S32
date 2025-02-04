/*
 * uart_bus.c
 *
 *  Created on: Feb 4, 2025
 *      Author: Orsted
 */

#include"uart_bus.h"

extern UART_HandleTypeDef huart1;

uint8_t UART1_RxBuffer[UART_RX_BUFFER_SIZE] = {0};

void initOneWireTransmission(){
	HAL_HalfDuplex_EnableReceiver(&huart1);
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, UART1_RxBuffer, UART_RX_BUFFER_SIZE);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, UART1_RxBuffer, UART_RX_BUFFER_SIZE);
}
