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
#include "../../MotorDriver/Inc/TMC2209.h"
#include "watcher.h"


void encoderCalibRun();
#endif /* INC_ENCODER_CALIB_H_ */
