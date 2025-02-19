/*
 * TMC2209.c
 *
 *  Created on: Feb 19, 2025
 *      Author: Orsted
 */
#include "../Inc/TMC2209.h"

TMC2209_Setup globalSetup;
extern TIM_HandleTypeDef htim2;
TMC2209_chopConfig chopConfig;
static uint8_t toff_ = TOFF_DEFAULT;
static uint8_t PWM_Pulse_Complete = TRUE;
static uint8_t Driver_Enable = FALSE;

void TMC2209_setup(TMC2209_Setup* setup)
{

	globalSetup.enablePin = setup->enablePin;
	globalSetup.stepPin = setup->stepPin;
	globalSetup.txPin = setup->txPin;

	TMC2209_disable();
}

void TMC2209_enable()
{
	if(!Driver_Enable){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
		Driver_Enable = TRUE;
		chopConfig.toff = toff_;
	}
}

void TMC2209_disable()
{
	if(Driver_Enable){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
		Driver_Enable = FALSE;
		chopConfig.toff = TOFF_DISABLE;
	}
}

void TMC2209_setMicrostep(TMC2209_Microstep Microstep)
{
	chopConfig.mres = Microstep;
	TMC2209_HAL_Write(TMC2209Reg_CHOPCONF, chopConfig.bytes);
}
void TMC2209_readChopConfig(uint32_t* result)
{
	uint32_t buffer;
	TMC2209_HAL_Read(TMC2209Reg_CHOPCONF, &buffer);
	*result = buffer;
}
void TMC2209_moveVelocity(uint8_t velocity)
{
	if(velocity == 0) {
		HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
		PWM_Pulse_Complete = TRUE;
		return;
	}
	if(PWM_Pulse_Complete){
		HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
		__HAL_TIM_SET_AUTORELOAD(&htim2, velocity);
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


