/*
 * watcher.h
 *
 *  Created on: Mar 28, 2025
 *      Author: Orsted
 */

#ifndef INC_WATCHER_H_
#define INC_WATCHER_H_
#include "stm32f1xx_hal.h"
#include <stdlib.h>
#include <stdbool.h>

#define WATCHER_BUFFER 50
#define TRUE 1
#define FALSE 0

typedef enum {
	WATCHER_ERR_NORM 			= 0,
	WATCHER_ERR_DRV_INIT 		= 1,
	WATCHER_ERR_DRV_FAULT 		= 2,

	WATCHER_ERR_ENC_DISCONNECT 	= 10,
	WATCHER_ERR_ENC_INIT 		= 11,
	WATCHER_ERR_ENC_LINE  		= 12,
	WATCHER_ERR_ENC_CALIBFLT 	= 13,

} WATCHER_ERR_CODE;


void initWatcherStack();
void pushError(WATCHER_ERR_CODE);
void removeError(WATCHER_ERR_CODE);
bool isErrorExist();
void displaySysStat();

#endif /* INC_WATCHER_H_ */
