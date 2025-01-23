/*
 * driver.c
 *
 *  Created on: Jan 23, 2025
 *      Author: Orsted
 */

#include "Driver.h"

volatile uint32_t pulseCount = 0;
volatile uint32_t target_count = 0;

void stepperInit(void) {
  HAL_GPIO_WritePin(GPIOA, DRV_ENA, GPIO_PIN_RESET);	// DRV ENABLE = 0 (on)
  HAL_GPIO_WritePin(GPIOA, DRV_SLEEP, GPIO_PIN_SET);  // DRV SLEEP = 1 (wake up)
  HAL_GPIO_WritePin(GPIOA, DRV_RSET, GPIO_PIN_SET);  // DRV RST = 1 (reset)
  HAL_Delay(10);
}
void setMicrostep(uint8_t mod2, uint8_t mod1, uint8_t mod0) {
	HAL_GPIO_WritePin(GPIOA, DRV_MOD2, mod2 ? GPIO_PIN_SET : GPIO_PIN_RESET); // MOD2
  HAL_GPIO_WritePin(GPIOA, DRV_MOD1, mod1 ? GPIO_PIN_SET : GPIO_PIN_RESET); // MOD1
  HAL_GPIO_WritePin(GPIOA, DRV_MOD0, mod0 ? GPIO_PIN_SET : GPIO_PIN_RESET); // MOD0
	microstep_factor = 1;
}
void generateStep(TIM_HandleTypeDef *htim, int stepCount){
	if(gen_complete == 1){
		pulseCount = 0;
		target_count = stepCount;
		HAL_TIM_PWM_Start_IT(htim, TIM_CHANNEL_1);
		gen_complete = 0;
	}
}
void configureSpeed(TIM_HandleTypeDef *htim, float rps){
	float step_per_rev = 360.0 / 1.8;
	step_per_rev = step_per_rev * microstep_factor;
	float freq = 0;
	freq = rps * step_per_rev;
	freq = PRESCALLER_FREQ / freq;
	 __HAL_TIM_SET_AUTORELOAD(htim, freq);

}
uint8_t gotoAngle(TIM_HandleTypeDef *htim, float* target){
	extern I2C_HandleTypeDef hi2c1;
	float sudut;
	while(1){
		generateStep(htim, 1);
		encGetAngle(&hi2c1, &sudut);
		if(sudut == *target){
			break;
		}
	}
	return 0;
}

// Intterupt function
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
			pulseCount++;
			if(pulseCount >= target_count){
				HAL_TIM_PWM_Stop_IT(htim, TIM_CHANNEL_1);
				pulseCount = 0;
				gen_complete = 1;
			}
    }
}

void watchPosition(){

}
