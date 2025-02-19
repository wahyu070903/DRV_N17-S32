/*
 * TMC2209_HAL.c
 *
 *  Created on: Feb 19, 2025
 *      Author: Orsted
 */

#include "../Inc/TMC2209_HAL.h"

extern UART_HandleTypeDef huart1;

static void calculateCRC(uint8_t* datagram, uint8_t datagram_len)
{
	int i,j;
	uint8_t *crc = datagram + (datagram_len - 1);
	uint8_t currentByte;
	*crc = 0;
	for (i = 0; i < (datagram_len - 1); i++) {
		currentByte = datagram[i];
		for (j = 0; j < 8; j++) {
			if ((*crc >> 7) ^ (currentByte & 0x01))
				*crc = (*crc << 1) ^ 0x07;
			else
				*crc = (*crc << 1);
			currentByte = currentByte >> 1;
		}
	}
}
static void byteswap(uint8_t data[4])
{
	uint8_t temp = data[0];
	data[0] = data[3];
	data[3] = temp;
	temp = data[1];
	data[1] = data[2];
	data[2] = temp;
}
HAL_StatusTypeDef TMC2209_HAL_Write(uint8_t reg, uint32_t data)
{
	TMC2209_Write_Datagram_t datagram;
	datagram.message.sync = 0x05;
	datagram.message.slave = 0x00;
	datagram.message.address = (reg << 1) | 0x01;
	datagram.message.payload.value = data;
	byteswap(datagram.message.payload.data);

	calculateCRC(datagram.bytes, sizeof(TMC2209_Write_Datagram_t));
	HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, datagram.bytes, sizeof(TMC2209_Write_Datagram_t), HAL_MAX_DELAY);

	return status;
}
HAL_StatusTypeDef TMC2209_HAL_Read(uint8_t reg, uint32_t* result)
{
	TMC2209_Read_Datagram_t datagram;
	TMC2209_Write_Datagram_t res;

	datagram.message.sync = 0x05;
	datagram.message.slave = 0x00;
	datagram.message.address = (reg << 1) & 0x00;
	calculateCRC(datagram.bytes, sizeof(TMC2209_Read_Datagram_t));

	HAL_StatusTypeDef wstatus = HAL_UART_Transmit(&huart1, datagram.bytes, sizeof(TMC2209_Write_Datagram_t), HAL_MAX_DELAY);
	if(wstatus == HAL_OK){
		HAL_Delay(10);
		uint8_t buffer[8] = {0};
		HAL_StatusTypeDef rstatus = HAL_UART_Receive(&huart1, buffer, sizeof(buffer), 1000);
		if(rstatus == HAL_OK){
			memcpy(res.bytes, buffer, sizeof(buffer));
			uint8_t crc = res.message.crc;
			calculateCRC(res.bytes, sizeof(	TMC2209_Write_Datagram_t));
			if(crc == res.message.crc){
				uint8_t temp_var[4];
				memcpy(temp_var,res.message.payload.data, sizeof(temp_var));
				byteswap(temp_var);
				*result = *(uint32_t*)temp_var;
			}
		}
	}
	return wstatus;
}

