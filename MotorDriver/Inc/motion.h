/*
 * motion.h
 *
 *  Created on: Jul 10, 2025
 *      Author: Orsted
 */

#ifndef INC_MOTION_H_
#define INC_MOTION_H_

#include "tmc2209.h"
#include "stm32f1xx_hal.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define TMC2209_BASE_FREQ 72000000	// 72MHZ
#define TMC2209_DEFAULT_PRESCALLER 72
#define MAX_CNT_PERIOD 65535
#define STEP_PER_REV 200
#define CONST_KP 100
#define CONST_KI 30
#define CONST_KD 0
#define MIN_SPEED 0
#define MAX_SPEED 120
#define PID_INTEGRAL_MAX 1000000
#define PID_INTEGRAL_MIN -1000000
#define PID_DT 0.001f  // Sampling time in seconds
#define PID_OUTPUT_MAX 1000000
#define PID_OUTPUT_MIN -1000000
#define NEAR_TARGET_ZONE 100
#define DEADBAND 1

typedef struct  {
	float Kp;
	float Ki;
	float Kd;

	int32_t setpoint;
	int32_t processVal;
	int32_t error;
	float output;
	float integral;
	float prevError;
} PID_t;

enum tmc2209_rot_direction_t {
	TMC2209_ROT_FWD = 0x00,
	TMC2209_ROT_REV = 0x01
};

void Motion_Init();
void Motion_Runtime();
float PID_Compute(int32_t*, int32_t*);

#endif /* INC_MOTION_H_ */
