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
TMC2209_ihold_irun_reg_t iholdIrun;
TMC2209_pwmconf_reg_t pwmConfig;

static uint8_t toff_ = TOFF_DEFAULT;
static uint8_t PWM_Pulse_Complete = TRUE;
static uint8_t Driver_Enable = FALSE;
static uint8_t active_microstep;
static float vel_now = 0.0;
static uint8_t rotation_dir;
static uint32_t stepCounter = 0;
static float pid_error = 0;
static float pid_last_error = 0;
static float pid_integral = 0;
static float pid_direvative = 0;
static float pid_output = 0;
static float pid_fraction = 0;
static uint32_t pid_last_time = 0;

static uint8_t oneStepMove_finish = FALSE;
static uint8_t oneStepMove_start = FALSE;

static uint8_t isBrake = FALSE;
void TMC2209_setdefault()
{
	gconfConfig.I_scale_analog = TRUE;
	gconfConfig.multistep_filt = TRUE;
	chopConfig.bytes = CHOPPER_CONFIG_DEFAULT;
	pwmConfig.bytes = PWMCONF_CONFIG_DEFAULT;
	iholdIrun.ihold = TMC_IRUNDEFAULT;
	iholdIrun.iholddelay = TMC_IHOLDDELAY;
	iholdIrun.irun = TMC_IRUNDEFAULT;
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

	uint8_t timeout_cnt = 0;
	TMC2209_gconf_reg_t gconf_verif;
	while(timeout_cnt < TMC_SETUP_TIMEOUT){
		TMC2209_HAL_Write(TMC2209Reg_GCONF, gconfConfig.bytes);
		TMC2209_HAL_Write(TMC2209Reg_SLAVECONF, slaveConfig.bytes);

		TMC2209_HAL_Read(TMC2209Reg_GCONF, &gconf_verif.bytes);
		if(gconf_verif.bytes == gconfConfig.bytes) break;
		else{
			HAL_Delay(100);
		}
		timeout_cnt++;
	}
	if(timeout_cnt >= TMC_SETUP_TIMEOUT){
		WATCHER_ERR_STAT err = WATCHER_DRV_FAULT;
		emmitSysError(err);
	}

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
	active_microstep = pow(2, abs(Microstep - TMC2209_Microsteps_1));
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
	if(velocity < 0.01f) velocity = 0.01f;
	if(velocity <= 0) {
		HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
		PWM_Pulse_Complete = TRUE;
		return;
	}

	while(TRUE){
		frequency  = (velocity * (STEP_PER_REV * active_microstep)) / 60;
		if(((TMC2209_BASE_FREQ / prescaller) / frequency) > MAX_CNT_PERIOD){
			prescaller += TMC2209_DEFAULT_PRESCALLER;
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
	if(isBrake) return;
	if(PWM_Pulse_Complete == TRUE){
		HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
		PWM_Pulse_Complete = FALSE;
	}
}

void TMC2209_stop(){
	if(PWM_Pulse_Complete == FALSE){
		HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
		PWM_Pulse_Complete = TRUE;
	}
}
void TMC2209_brake(){
	// !NOTE nanti cek apa sudah mode spreadcycle
	if(!PWM_Pulse_Complete) return;
	if(!isBrake){
		iholdIrun.ihold = 0;
		pwmConfig.freewheel = TMC_FREEWHEEL_BRAKE;
		TMC2209_HAL_Write(TMC2209Reg_IHOLD_IRUN, iholdIrun.bytes);
		TMC2209_HAL_Write(TMC2209Reg_PWMCONF, pwmConfig.bytes);

		isBrake = TRUE;
	}
}
void TMC2209_unBrake(){
	if(isBrake){
		iholdIrun.ihold = 16;
		pwmConfig.freewheel = TMC_FREEWHEEL_NORMAL;
		TMC2209_HAL_Write(TMC2209Reg_IHOLD_IRUN, iholdIrun.bytes);
		TMC2209_HAL_Write(TMC2209Reg_PWMCONF, pwmConfig.bytes);

		isBrake = FALSE;
	}
}
void TMC2209_direction(uint8_t direction){
	if(direction == rotation_dir) return;

	if(direction == TMC2209_ROT_FWD){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
		rotation_dir = TMC2209_ROT_FWD;
	}
	if(direction == TMC2209_ROT_REV){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
		rotation_dir = TMC2209_ROT_REV;
	}
}

void PID_controller(int32_t* setpoint, int32_t *current, float* result){
	uint32_t time_now = HAL_GetTick();
	if(time_now - pid_last_time >= PID_SAMPLING){
		float delta_time = (float)(time_now - pid_last_time) / 1000.0f;
		pid_error = (float)(*setpoint - *current);

		if(fabs(pid_error) < PID_DEADBAND){
			*result = 0;
			pid_last_time = time_now;
			return;
		}

		if(fabs(pid_error) > PID_INTEGRAL_TRESHOLD){
			pid_integral += pid_error * delta_time;
			if(pid_integral > PID_INTEGRAL_MAX) pid_integral = PID_INTEGRAL_MAX;
			if(pid_integral < PID_INTEGRAL_MIN) pid_integral = PID_INTEGRAL_MIN;
		}else{
			pid_integral *= 0.9f;
		}

		float direvative = (pid_error - pid_last_error) / delta_time;
		pid_direvative = (pid_direvative * 0.9f) + (direvative * 0.1f);

		pid_output = (PID_KP * pid_error) + (PID_KI * pid_integral) + (PID_KD * pid_direvative);

		pid_last_error = pid_error;
		pid_last_time = time_now;

		*result = fmaxf(0.0f, fminf(1.0f, fabs(pid_output / PID_MAX)));
	}
}
void TMC2209_watchPosition(int32_t* target, int32_t* counter, float* speed){

	PID_controller(target, counter, &pid_fraction);

	int32_t error = *target - *counter;
	if(error == 0){
		TMC2209_stop();
		*speed = 0;
		return;
	}else{
		TMC2209_move();
	}

	if(*counter > *target) TMC2209_direction(TMC2209_ROT_FWD);
	if(*counter < *target) TMC2209_direction(TMC2209_ROT_REV);

	if(abs(*target - *counter) > 100){
		if(active_microstep != TMC2209_Microsteps_1){
			TMC2209_setMicrostep(TMC2209_Microsteps_1);
		}
	}else{
		if(active_microstep != TMC2209_Microsteps_64){
			TMC2209_setMicrostep(TMC2209_Microsteps_64);
		}
	}

	*speed = pid_fraction * MAX_SPEED;
	TMC2209_velocity(*speed);
}

void TMC2209_getDirection(uint8_t* result){
	*result = rotation_dir;
}

void TMC2209_rotateOnce(){
	uint32_t target = STEP_PER_REV * active_microstep;
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
	while(TRUE){
		if(stepCounter >= target){
			HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
			stepCounter = 0;
			break;
		}
	}
}

void TMC2209_moveOneStep(){
	oneStepMove_finish = FALSE;
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
	oneStepMove_start = TRUE;
	while(!oneStepMove_finish);

	HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);

}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2) {
		if(oneStepMove_start){
			oneStepMove_finish = TRUE;
			oneStepMove_start = FALSE;
		}

		stepCounter ++;
	}
}


