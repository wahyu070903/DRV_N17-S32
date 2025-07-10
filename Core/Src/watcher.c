/*
 * watcher.c
 *
 *  Created on: Mar 28, 2025
 *      Author: Orsted
 */

#include "watcher.h"

WATCHER_t watcher;

void emmitSysError(WATCHER_ERR_STAT error){
	watcher.detail = WATCHER_ERROR;
	watcher.stat = error;
};

uint8_t getSysStatus(){
	return watcher.detail;
};

uint8_t getSysError(){
	return watcher.stat;
}
void displaySysStat(){
	if(watcher.detail == WATCHER_ERROR){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_Delay(100);
	}else{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
	}

}
