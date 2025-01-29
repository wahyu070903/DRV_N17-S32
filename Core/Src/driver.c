/*
 * driver.c
 *
 *  Created on: Jan 23, 2025
 *      Author: Orsted
 */

#include "driver.h"
#include "math.h"
#include <stdlib.h>

extern TIM_HandleTypeDef htim2;
volatile uint32_t target_count = 0;
float microstep_factor = 0;
static uint8_t pulse_complete = 1;
static uint8_t rotation_direction = 0;		// 0 = CCW, 1 = CW
float pid_integral = 0.0;
float pid_error_prev = 0.0;

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

	if (mod2 == 0 && mod1 == 0 && mod0 == 0) {
	    microstep_factor = 1.0;
	}else if (mod2 == 0 && mod1 == 0 && mod0 == 1) {
	    microstep_factor = 1.0 / 2.0;
	}else if (mod2 == 0 && mod1 == 1 && mod0 == 0) {
	    microstep_factor = 1.0 / 4.0;
	}else if (mod2 == 0 && mod1 == 1 && mod0 == 1) {
	    microstep_factor = 8.0;
	}else if (mod2 == 1 && mod1 == 0 && mod0 == 0){
		microstep_factor = 16.0;
	}else if (mod2 == 1) {
	    microstep_factor = 32.0;
	}
}

void setDirection(uint8_t direction){
	if(direction == 0){
		// Counter clockwise
		HAL_GPIO_WritePin(GPIOA, DRV_DIR, GPIO_PIN_RESET);
		rotation_direction = 0;
	}else if (direction == 1){
		// Clockwise
		HAL_GPIO_WritePin(GPIOA, DRV_DIR, GPIO_PIN_SET);
		rotation_direction = 1;
	}
}

uint8_t getDirection(void){
	return rotation_direction;
}

void generateStep(){
	if(pulse_complete == 1){
		HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
		pulse_complete = 0;
	}
}

void configureSpeed(float rps){
	float step_per_rev = 360.0 / 1.8;
	step_per_rev = step_per_rev * microstep_factor;
	float freq = rps * step_per_rev;
	uint32_t timer_reload = (uint32_t)(PRESCALLER_FREQ / freq);
	if(PRESCALLER_FREQ / timer_reload > MAX_FREQ){
		timer_reload = (uint32_t)(PRESCALLER_FREQ / MAX_FREQ) + 1;
		// +1 important to ceil up number
	}
	 __HAL_TIM_SET_AUTORELOAD(&htim2, timer_reload);

}

void maintainPosition(int32_t target, int32_t* current_pos, float* watch){
	if(target != *current_pos){
		// PID
//		float error = (float)(target - *current_pos);
//		pid_integral += error;
//		float derivative = error - pid_error_prev;
//		float output = (KP * error) + (KI * pid_integral) + (KD * derivative);
//		pid_error_prev = error;
//		*watch = output;
//
		if(target > *current_pos) setDirection(1);
		else if(target < *current_pos) setDirection(0);
		generateStep();
	}
}

void motion(int32_t target, int32_t* pos_now, uint8_t* init_flag, float* start_time){
	int32_t distance = abs(target - *pos_now - 1000);
	static float Tj = 0;
	static float Ta = 0;
	static float Tv = 0;
	static float time_profile[7] = {0};
	float velocity = 0;

	if(distance > 1000){
		if(*init_flag == 0){
			*start_time = HAL_GetTick() / 1000;
			*init_flag = 1;
		}

		Tj = MAX_ACC / MAX_JERK;
		Ta = MAX_VEL / MAX_ACC - MAX_ACC / MAX_JERK;
		Tv = ((abs(target - *pos_now)/4096) / MAX_VEL) - (MAX_VEL / MAX_ACC) - (MAX_ACC / MAX_JERK);
		time_profile[0] = Tj;
		time_profile[1] = Tj + Ta;
		time_profile[2] = 2*Tj + Ta;
		time_profile[3] = 2*Tj + Ta + Tv;
		time_profile[4] = 3*Tj + Ta + Tv;
		time_profile[5] = 3*Tj + 2*Ta + Tv;
		time_profile[6] = 4*Tj + 2*Ta + Tv;

		float time_now = (HAL_GetTick() / 1000) - *start_time;

		if(time_now <= time_profile[0]){
			velocity = MAX_JERK / 2 * (time_now * time_now);
		}else if(time_now <= time_profile[1]){
			velocity = (0.5 * MAX_ACC * Tj) + (MAX_ACC * (time_now - time_profile[0]));
		}else if(time_now <= time_profile[2]){
			velocity = MAX_ACC * (Tj / 2 + Ta) + (MAX_ACC * (time_now - time_profile[1])) - (0.5 * MAX_JERK * pow(time_now - time_profile[1], 2));
		}else if(time_now <= time_profile[3]){
			velocity = MAX_VEL;
		}else if(time_now <= time_profile[4]){
			velocity = MAX_VEL - ((0.5 * MAX_JERK * pow(time_now - time_profile[3], 2)));
		}else if(time_now <= time_profile[5]){
			velocity = MAX_VEL - (0.5 * MAX_ACC * Tj) - (MAX_ACC * (time_now - time_profile[4]));
		}else if(time_now <= time_profile[6]){
			velocity = (0.5 * MAX_ACC * Tj) - (MAX_ACC * (time_now - time_profile[5])) + (0.5 * MAX_JERK * pow(time_now - time_profile[5], 2));
		}else{
			*init_flag = 0;
		}

		setMicrostep(0,0,0);
		configureSpeed(velocity);

	}else{
		setMicrostep(1,1,1);
		configureSpeed(0.4);
	}
}

// Interrupt function
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
    	HAL_TIM_PWM_Stop_IT(htim, TIM_CHANNEL_1);
    	pulse_complete = 1;
    }
}

