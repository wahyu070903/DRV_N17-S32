/*
 * encoder_calib.h
 *
 *  Created on: Mar 31, 2025
 *      Author: Orsted
 */

#ifndef INC_ENCODER_CALIB_H_
#define INC_ENCODER_CALIB_H_

#include "stm32f1xx_hal.h"
#include "Encoder.h"
#include "watcher.h"
#include "../../MotorDriver/Inc/tmc2209.h"


void encoderCalibRun();
#endif /* INC_ENCODER_CALIB_H_ */
