/*
 * watcher.c
 *
 *  Created on: Mar 28, 2025
 *      Author: Orsted
 */

#include "watcher.h"

WATCHER_ERR_CODE watcherList [WATCHER_BUFFER];
uint8_t listPointer;
uint8_t errorCounter;
uint8_t warnCounter;

void initWatcherStack(){
	for(int i = 0; i < WATCHER_BUFFER ; i++){
		watcherList[i] = WATCHER_ERR_NORM;
	}

	listPointer = 0;
	errorCounter = 0;
	warnCounter = 0;
}

void pushError(WATCHER_ERR_CODE errCode){
	if(listPointer >= WATCHER_BUFFER) return;
	for(uint8_t i = 0 ; i < WATCHER_BUFFER ; i++){
		if(watcherList[i] == errCode) return;
	}
	watcherList[listPointer] = errCode;
	listPointer++;
	errorCounter++;
}


void removeError(WATCHER_ERR_CODE errCode){
	if(listPointer <= 0) return;
	for(uint8_t i = 0; i < WATCHER_BUFFER ; i++) {
		if(watcherList[i] == errCode){
			watcherList[i] = WATCHER_ERR_NORM;
		}
	}
	listPointer--;
	errorCounter--;
}

bool isErrorExist(){
	if(errorCounter > 0) return TRUE;
	return FALSE;
}

void displaySysStat(){
	if(isErrorExist()){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_Delay(100);
	}else{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
	}

}
