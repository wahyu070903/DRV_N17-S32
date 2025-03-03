/*
 * TMC2209.c
 *
 *  Created on: Feb 19, 2025
 *      Author: Orsted
 */
#include "../Inc/TMC2209.h"
#include <math.h>
#include <stdlib.h>

TMC2209_Setup globalSetup;
extern TIM_HandleTypeDef htim2;
TMC2209_chopConfig chopConfig;
TMC2209_gconf_reg_t gconfConfig;
TMC2209_slaveconf_reg_t slaveConfig;

static uint8_t toff_ = TOFF_DEFAULT;
static uint8_t PWM_Pulse_Complete = TRUE;
static uint8_t Driver_Enable = FALSE;
static uint8_t active_microstep;
static float vel_now = 0.0;

void TMC2209_setdefault()
{
	gconfConfig.I_scale_analog = TRUE;
	gconfConfig.multistep_filt = TRUE;
	chopConfig.bytes = CHOPPER_CONFIG_DEFAULT;
}

void TMC2209_setup()
{
	TMC2209_setdefault();
	gconfConfig.bytes = FALSE;
	gconfConfig.I_scale_analog = TRUE;
	gconfConfig.pdn_disable = TRUE;
	gconfConfig.multistep_filt = TRUE;
	gconfConfig.mstep_reg_select = TRUE;

	slaveConfig.conf = 0x00;

	TMC2209_HAL_Write(TMC2209Reg_GCONF, gconfConfig.bytes);
	TMC2209_HAL_Write(TMC2209Reg_SLAVECONF, slaveConfig.bytes);
//	TMC2209_setMicrostep(TMC2209_Microsteps_1);
	TMC2209_disable();
	HAL_Delay(100);
}

void TMC2209_enable()
{
	if(Driver_Enable == FALSE){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
		Driver_Enable = TRUE;
		chopConfig.toff = toff_;
		TMC2209_HAL_Write(TMC2209Reg_CHOPCONF, chopConfig.bytes);
	}
}

void TMC2209_disable()
{
	if(Driver_Enable == TRUE){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
		Driver_Enable = FALSE;
		chopConfig.toff = TOFF_DISABLE;
		TMC2209_HAL_Write(TMC2209Reg_CHOPCONF, chopConfig.bytes);
	}
}

void TMC2209_setMicrostep(TMC2209_Microstep Microstep)
{
	chopConfig.mres = Microstep;
	TMC2209_HAL_Write(TMC2209Reg_CHOPCONF, chopConfig.bytes);
	active_microstep = 2 ^ abs(Microstep - TMC2209_Microsteps_1);
}
void TMC2209_readChopConfig(uint32_t* result)
{
	uint32_t buffer = 0;
	TMC2209_HAL_Read(TMC2209Reg_CHOPCONF, &buffer);
	*result = buffer;
}

void TMC2209_velocity(float velocity)
{
	float frequency = 0.0f;
	uint16_t desired_period = 0;
	uint16_t prescaller = TMC2209_DEFAULT_PRESCALLER;

	if(vel_now == velocity) return;
	if(velocity <= 0) {
		HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
		PWM_Pulse_Complete = TRUE;
		return;
	}

	while(TRUE){
		frequency  = (velocity * (STEP_PER_REV * active_microstep)) / 60;
		if(((TMC2209_BASE_FREQ / prescaller) / frequency) > MAX_CNT_PERIOD){
			prescaller = prescaller * 2;
			continue;
		}
		desired_period = (uint16_t)round((TMC2209_BASE_FREQ / prescaller) / frequency);
		break;
	}

	__HAL_TIM_SET_PRESCALER(&htim2, prescaller);
	__HAL_TIM_SET_AUTORELOAD(&htim2, desired_period);
	vel_now = velocity;
}

void TMC2209_move(){
	if(PWM_Pulse_Complete == TRUE){
		HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
		PWM_Pulse_Complete = FALSE;
	}
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2) {
		HAL_TIM_PWM_Stop_IT(htim, TIM_CHANNEL_1);
		PWM_Pulse_Complete = TRUE;
	}
}


