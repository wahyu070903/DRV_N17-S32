/*
 * driver.h
 *
 *  Created on: Jan 23, 2025
 *      Author: Orsted
 */

#ifndef INC_DRIVER_H_
#define INC_DRIVER_H_

	#include "stm32f1xx_hal.h"

	#define DRV_STEP 		GPIO_PIN_0
	#define DRV_DIR			GPIO_PIN_1
	#define DRV_MOD0		GPIO_PIN_2
	#define DRV_MOD1		GPIO_PIN_3
	#define DRV_MOD2		GPIO_PIN_4
	#define DRV_ENA			GPIO_PIN_5
	#define DRV_FAULT		GPIO_PIN_6
	#define DRV_SLEEP		GPIO_PIN_7
	#define DRV_RSET		GPIO_PIN_8

	#define STEP_ANGLE 1.8
	#define MIN_FREQ 0			// 0Hz
	#define MAX_FREQ 18000		// 18Khz
	#define PRESCALLER_FREQ 1000000	//1Mhz

	#define KP 2.0f
	#define KI 0.5f
	#define KD 0.1f
	#define OFFSET 100
	#define MAX_JERK 0.2f
	#define MAX_ACC	0.8f
	#define MAX_VEL	5.0f

	void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);
	void stepperInit(void);
	void setMicrostep(uint8_t, uint8_t, uint8_t);
	void setDirection(uint8_t);
	uint8_t getDirection(void);
	void configureSpeed(float);
	void generateStep();
	uint8_t generateSteps(uint8_t);
	void resetPulseCount();
	void maintainPosition(int32_t, int32_t*, float*);
	void motion(int32_t, int32_t*, uint8_t*, float*);

#endif /* INC_DRIVER_H_ */
