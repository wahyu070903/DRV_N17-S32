/*
 * driver.c
 *
 *  Created on: Jan 23, 2025
 *      Author: Orsted
 */

#include "driver.h"

extern UART_HandleTypeDef huart1;

// this calc_crc code provided by datasheet
static void calcCRC (uint8_t *datagram, uint8_t datagramLength)
{
    int i,j;
    uint8_t *crc = datagram + (datagramLength - 1); // CRC located in last byte of message
    uint8_t currentByte;
    *crc = 0;
    for (i = 0; i < (datagramLength - 1); i++) {    // Execute for all bytes of a message
        currentByte = datagram[i];                  // Retrieve a byte to be sent from Array
        for (j = 0; j < 8; j++) {
            if ((*crc >> 7) ^ (currentByte & 0x01)) // update CRC based result of XOR operation
                *crc = (*crc << 1) ^ 0x07;
            else
                *crc = (*crc << 1);
            currentByte = currentByte >> 1;
        } // for CRC bit
    }
}

static void byteswap(uint8_t* data){
	uint8_t temp;
	temp = data[0];
	data[0] = data[3];
	data[3] = temp;
	temp = data[1];
	data[1] = data[2];
	data[2] = temp;
}

HAL_StatusTypeDef DRV_ReadRegister(uint8_t reg, uint32_t* value){
	uint8_t request[4];
	request[0] = 0x05;  // Slave Address (Default 0x05)
	request[1] = 0x00;
	request[2] = reg & 0x7F;  // Read command
	calcCRC(request, 4);

	//HAL_HalfDuplex_EnableTransmitter(&huart1);
	HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, request, 4, 500);
	if (status != HAL_OK) return status;

	HAL_Delay(2);

	//HAL_HalfDuplex_EnableReceiver(&huart1);
	uint8_t response[8];
	status = HAL_UART_Receive(&huart1, response, 8, 500);
	if (status != HAL_OK) return status;

	*value = (response[4] << 24) | (response[5] << 16) | (response[6] << 8) | response[7];
	return HAL_OK;
}

HAL_StatusTypeDef DRV_WriteRegister(uint8_t reg, uint32_t value) {

    uint8_t data[8];
    data[0] = 0x05;  // Slave Address
    data[1] = 0x00;  // Write Command
    data[2] = reg | 0x80;
    data[3] = (value >> 24) & 0xFF;
    data[4] = (value >> 16) & 0xFF;
    data[5] = (value >> 8) & 0xFF;
    data[6] = value & 0xFF;
    calcCRC(data, 8);

    HAL_StatusTypeDef status;
    status =  HAL_UART_Transmit(&huart1, data, 8, 100);
    return status;
}

void tmc2209_init() {
	HAL_StatusTypeDef status;
	status = DRV_WriteRegister(0x00, 0x00000004);  // Set PDN_DISABLE in GCONF
	H_Delay(100);
}
