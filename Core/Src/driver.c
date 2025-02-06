/*
 * driver.c
 *
 *  Created on: Jan 23, 2025
 *      Author: Orsted
 */

#include "driver.h"

extern UART_HandleTypeDef huart1;

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

static HAL_StatusTypeDef TMC_ReadRegister(TMC2209_read_datagram_t* datagram, uint32_t* value){

	datagram->msg.addr = datagram->msg.addr & 0x7F;
	HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, datagram->data, sizeof(datagram), 100);
	if (status != HAL_OK) return status;

	HAL_Delay(2);

	uint8_t response[8];
	status = HAL_UART_Receive(&huart1, response, sizeof(response), 100);
	if (status != HAL_OK) return status;

	*value = (response[4] << 24) | (response[5] << 16) | (response[6] << 8) | response[7];
	return HAL_OK;
}

static HAL_StatusTypeDef TMC_WriteRegister(TMC2209_write_datagram_t* datagram){

	datagram->msg.addr = datagram->msg.addr | 0x80;
	HAL_StatusTypeDef status;
	status =  HAL_UART_Transmit(&huart1, datagram->data, sizeof(datagram), 100);
	return status;
}

static void byteswap(uint8_t* bytes){
	uint8_t temp = bytes[0];
	bytes[0] = bytes[3];
	bytes[3] = temp;
	temp = bytes[1];
	bytes[1] = bytes[2];
	bytes[2] = temp;
}

void tmc2209_init() {
	TMC2209_write_datagram_t gconf_dat;
	tmc2209_regaddr_t gconf_reg = TMC2209Reg_GCONF;
	TMC2209_payload gconf_payload;

	gconf_dat.msg.sync = 0x05;
	gconf_dat.msg.slave = TMC2209_SLAVE_ADDR;
	gconf_dat.msg.addr = gconf_reg;

	gconf_payload.gconf.pdn_disable = 1;
	gconf_dat.msg.payload = gconf_payload;
	byteswap(gconf_dat.msg.payload.data);

	calcCRC(gconf_dat.data, sizeof(TMC2209_write_datagram_t));

	TMC_WriteRegister(&gconf_dat);  // Set PDN_DISABLE in GCONF
	HAL_Delay(100);
}

void DRV_readgconf(uint32_t* result){

	TMC2209_read_datagram_t datagram;
	tmc2209_regaddr_t reg = TMC2209Reg_GCONF;

	datagram.msg.sync = 0x05;
	datagram.msg.slave = TMC2209_SLAVE_ADDR;
	datagram.msg.addr = reg;
	calcCRC(datagram.data, sizeof(TMC2209_read_datagram_t));

	TMC_ReadRegister(&datagram, result);
}
