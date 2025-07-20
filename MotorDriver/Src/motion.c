/*
 * motion.c
 *
 *  Created on: Jul 10, 2025
 *      Author: Orsted
 */

#include "../Inc/motion.h"

TMC2209_t driver;
PID_t PIDController;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern float pid_computed;

static uint8_t rotation_dir;
static uint16_t current_read = 0;
static float vel_now = 0.0;
static uint8_t active_microstep;
static uint8_t PWM_Pulse_Complete = TRUE;
static uint8_t isBrake = FALSE;
static float pid2velocity = 0;

void PID_Init(PID_t* controller, float Kp, float Ki, float Kd){
	controller->Kp = Kp;
	controller->Ki = Ki;
	controller->Kd = Kd;

	controller->integral = 0;
	controller->prevError = 0;
	controller->output = 0;
}

void PID_Run(){
	HAL_TIM_Base_Start_IT(&htim3);
}

void PID_Stop(){
	HAL_TIM_Base_Stop_IT(&htim3);
}

float PID_Compute(int32_t* sp, int32_t* pv){
	PIDController.setpoint = *sp;
	PIDController.processVal = *pv;
	PIDController.error = PIDController.setpoint - PIDController.processVal;

	// Integrasi error dikalikan dt
	PIDController.integral += PIDController.error * PID_DT;

	// Clamp anti-windup
	if (fabs(PIDController.error) == 0) {
	    PIDController.integral = 0;
	}
	if(PIDController.integral > PID_INTEGRAL_MAX) PIDController.integral = PID_INTEGRAL_MAX;
	if(PIDController.integral < PID_INTEGRAL_MIN) PIDController.integral = PID_INTEGRAL_MIN;

	// Derivatif dibagi dt
	float derivative = (PIDController.error - PIDController.prevError) / PID_DT;

	float output = PIDController.Kp * PIDController.error
				 + PIDController.Ki * PIDController.integral
				 + PIDController.Kd * derivative;

	// Clamp output (optional)
	if(output > PID_OUTPUT_MAX) output = PID_OUTPUT_MAX;
	if(output < PID_OUTPUT_MIN) output = PID_OUTPUT_MIN;

	PIDController.prevError = PIDController.error;
	PIDController.output = output;

	return output;
}

void Motion_Init(){
	TMC2209_SetDefaults(&driver);
	TMC2209_Init(&driver);
	PID_Init(&PIDController, CONST_KP, CONST_KI, CONST_KD);
}
void Motion_Move(){
	if(isBrake) return;
	if(PWM_Pulse_Complete == TRUE){
		HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
		PWM_Pulse_Complete = FALSE;
	}
}

void Motion_Stop(){
	if(PWM_Pulse_Complete == FALSE){
		HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
		PWM_Pulse_Complete = TRUE;
	}
}

void Motion_Direction(uint8_t direction){
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

void Motion_SetMicrostep(tmc2209_microsteps_t ustep){
	TMC2209_SetMicrosteps(&driver, ustep);
	active_microstep = ustep;
}

void Motion_Velocity(float velocity)
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

float _scale(float x, float in_min, float in_max, float out_min, float out_max) {
    return ((x - in_min) * (out_max - out_min) / (in_max - in_min)) + out_min;
}

void Motion_Runtime(){
    current_read = TMC2209_GetCurrent(&driver, TMCCurrent_Actual);

    PID_Run();  // harus dipanggil dulu agar PIDController.error ter-update

    float abs_error = fabs(PIDController.error);
    float abs_output = abs(pid_computed);

    // Ganti microstep berdasarkan error
    if(abs_error < NEAR_TARGET_ZONE){
        Motion_SetMicrostep(TMC2209_Microsteps_8);
    }else{
        Motion_SetMicrostep(TMC2209_Microsteps_1);
    }

    // Arah motor
    if(PIDController.error >= 0){
        Motion_Direction(TMC2209_ROT_REV);
    }else{
        Motion_Direction(TMC2209_ROT_FWD);
    }

    // Cek deadband + output minimum
    if(abs_error < DEADBAND) {
        Motion_Stop();
        return;
    }

    // Gerakkan motor
    Motion_Move();
    pid2velocity = _scale(abs_output, 0, PID_OUTPUT_MAX, MIN_SPEED, MAX_SPEED);
    Motion_Velocity(pid2velocity);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2) {
		if(PWM_Pulse_Complete == FALSE){
			PWM_Pulse_Complete = TRUE;
		}
	}
}
