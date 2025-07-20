/*
 * encoder_calib.c
 *
 *  Created on: Mar 31, 2025
 *      Author: Orsted
 */

#include "../Inc/encoder_calib.h"
uint8_t calib_plant = 200;

uint8_t encoderCalib20KhzRun();

void encoderCalibRun(){
//	TMC2209_setMicrostep(TMC2209_Microsteps_1);
//	TMC2209_enable();
//	TMC2209_velocity(3.2);
	uint8_t cnt_cycle = 0;
	uint8_t acc_cycle = 0;
	uint8_t strong_cycle = 0;
	uint8_t weak_cycle = 0;

	for(uint8_t i = 0 ; i < calib_plant ; i++){
		uint8_t mag_stat[3] = {0};
//		TMC2209_moveOneStep();
		HAL_StatusTypeDef status =  EncoderGetMagAlingment(mag_stat);
		if(status != HAL_OK || mag_stat[2] == 0){
			cnt_cycle++;
			continue;
		}
		if(mag_stat[0] == 1) strong_cycle++;
		if(mag_stat[1] == 1) weak_cycle++;
		acc_cycle++;
		cnt_cycle++;
	}

	float success_rate = (acc_cycle / cnt_cycle) * 100;

	if(success_rate < 100){
		pushError(WATCHER_ERR_ENC_CALIBFLT);
		return;
	}

//	TMC2209_direction(TMC2209_ROT_FWD);
//	uint8_t delta20Khz_fwd = encoderCalib20KhzRun();
//	TMC2209_direction(TMC2209_ROT_REV);
//	uint8_t delta20Khz_rev = encoderCalib20KhzRun();

}

uint8_t encoderCalib20KhzRun(){
	static uint8_t delta = 0;
	static uint16_t data_start = 0;
	EncoderGetAngle(&data_start);

//	TMC2209_setMicrostep(TMC2209_Microsteps_1);
//	TMC2209_velocity(3.2);
//	TMC2209_rotateOnce();

	static uint16_t data_end = 0;
	EncoderGetAngle(&data_end);

	delta = data_end - data_start;

	return delta;
}
