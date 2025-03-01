/*
 * TMC2209.h
 *
 *  Created on: Feb 19, 2025
 *      Author: Orsted
 */

#ifndef INC_TMC2209_H_
#define INC_TMC2209_H_

#pragma once
#include "stm32f1xx_hal.h"
#include "TMC2209_HAL.h"

#define TRUE 1
#define FALSE 0
#define TMC2209_FREQ 1000000	// 1KHZ
#define TOFF_DEFAULT 3
#define TOFF_DISABLE 0

const static uint32_t CHOPPER_CONFIG_DEFAULT = 0x10000053;

enum tmc2209_regaddr_t {
    TMC2209Reg_GCONF        = 0x00,
    TMC2209Reg_GSTAT        = 0x01,
    TMC2209Reg_IFCNT        = 0x02,
    TMC2209Reg_SLAVECONF    = 0x03,
    TMC2209Reg_OTP_PROG     = 0x04,
    TMC2209Reg_OTP_READ     = 0x05,
    TMC2209Reg_IOIN         = 0x06,
    TMC2209Reg_FACTORY_CONF = 0x07,

    TMC2209Reg_IHOLD_IRUN   = 0x10,
    TMC2209Reg_TPOWERDOWN   = 0x11,
    TMC2209Reg_TSTEP        = 0x12,
    TMC2209Reg_TPWMTHRS     = 0x13,
    TMC2209Reg_VACTUAL      = 0x22,
    TMC2209Reg_TCOOLTHRS    = 0x14,
    TMC2209Reg_SGTHRS       = 0x40,
    TMC2209Reg_SG_RESULT    = 0x41,
    TMC2209Reg_COOLCONF     = 0x42,

    TMC2209Reg_MSCNT        = 0x6A,
    TMC2209Reg_MSCURACT     = 0x6B,
    TMC2209Reg_CHOPCONF     = 0x6C,
    TMC2209Reg_DRV_STATUS   = 0x6F,
    TMC2209Reg_PWMCONF      = 0x70,
    TMC2209Reg_PWM_SCALE    = 0x71,
    TMC2209Reg_PWM_AUTO     = 0x72,
    TMC2209Reg_LAST_ADDR    = TMC2209Reg_PWM_AUTO
};

typedef struct {
	uint8_t enablePin;
	uint8_t stepPin;
	uint8_t txPin;
} TMC2209_Setup;

typedef union {
    uint32_t bytes;
    struct {
        uint32_t
        I_scale_analog   :1,
        internal_Rsense  :1,
        en_spreadcycle   :1,
        shaft            :1,
        index_otpw       :1,
        index_step       :1,
        pdn_disable      :1,
        mstep_reg_select :1,
        multistep_filt   :1,
        test_mode        :1,
        reserved         :22;
    };
} TMC2209_gconf_reg_t;

typedef union {
    uint32_t bytes;
    struct {
        uint32_t
        reserved0 :8,
        conf      :4,
        reserved1 :20;
    };
} TMC2209_slaveconf_reg_t;

typedef union {
	uint32_t bytes;
	struct {
		uint32_t
		toff 			: 4,
		hstart 			: 3,
		hend 			: 4,
		reserved_0		: 4,
		tbl				: 2,
		vsense			: 1,
		reserved_1 		: 6,
		mres 			: 4,
		interpolation 	: 1,
		double_edge 	: 1,
		diss2g 			: 1,
		diss2vs 		: 1;
	};
} TMC2209_chopConfig;

typedef enum {
	TMC2209_Microsteps_1 	= 0b1000,
	TMC2209_Microsteps_2 	= 0b0111,
	TMC2209_Microsteps_4 	= 0b0110,
	TMC2209_Microsteps_8 	= 0b0101,
	TMC2209_Microsteps_16 	= 0b0100,
	TMC2209_Microsteps_32 	= 0b0011,
	TMC2209_Microsteps_64 	= 0b0010,
	TMC2209_Microsteps_128 	= 0b0001,
	TMC2209_Microsteps_256 	= 0b0000,
} TMC2209_Microstep;

void TMC2209_setup();
void TMC2209_enable();
void TMC2209_disable();
void TMC2209_setMicrostep(TMC2209_Microstep);
void TMC2209_readChopConfig(uint32_t*);
void TMC2209_moveVelocity(uint8_t);

#endif /* INC_TMC2209_H_ */
