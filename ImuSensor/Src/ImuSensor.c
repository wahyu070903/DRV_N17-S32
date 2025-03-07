/*
 * ImuSensor.c
 *
 *  Created on: Mar 7, 2025
 *      Author: Orsted
 */


#include "../Inc/ImuSensor.h"

uint8_t save_buffer = 0;

ADXL_dummy_test(&save_buffer);
