/*
 * watcher.h
 *
 *  Created on: Mar 28, 2025
 *      Author: Orsted
 */

#ifndef INC_WATCHER_H_
#define INC_WATCHER_H_
#include "stm32f1xx_hal.h"

#define WATCHER_BUFFER 50

typedef enum {
	WATCHER_NORMAL =  0x00,
	WATCHER_ERROR  = 0x01,
} WATCHER_STAT;

typedef enum {
	WATCHER_DRV_FAULT = 0x00,
	WATCHER_ENC_FAULT = 0x01,
	WATCHER_IMU_FAULT = 0x02,
	WATCHER_CAN_FAULT = 0x03,
} WATCHER_ERR_STAT;

typedef struct{
	WATCHER_STAT stat;
	WATCHER_ERR_STAT detail;
} WATCHER_t;

uint8_t getSysStatus();
uint8_t getSysError();
void emmitSysError(WATCHER_ERR_STAT);
void resetSysError();
void displaySysStat();

#endif /* INC_WATCHER_H_ */
