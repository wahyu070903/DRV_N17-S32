/*
 * driver.h
 *
 *  Created on: Jan 23, 2025
 *      Author: Orsted
 */

#ifndef INC_DRIVER_H_
#define INC_DRIVER_H_

	#include <stdbool.h>
	#include "stm32f1xx_hal.h"

	// GPIO address
	#define DRV_STEP 			GPIO_PIN_0
	#define DRV_DIR				GPIO_PIN_1
	#define DRV_ENA				GPIO_PIN_2
	#define TMC2209_SLAVE_ADDR	0x00
	// default values

	// General
	#define TMC2209_F_CLK               12000000UL              // factory tuned to 12MHz - see datasheet for calibration procedure if required
	#define TMC2209_MODE                TMCMode_StealthChop     // 0 = TMCMode_StealthChop, 1 = TMCMode_CoolStep, 3 = TMCMode_StallGuard
	#define TMC2209_MICROSTEPS          TMC2209_Microsteps_4
	#define TMC2209_R_SENSE             110                     // mOhm
	#define TMC2209_CURRENT             500                     // mA RMS
	#define TMC2209_HOLD_CURRENT_PCT    50

	// CHOPCONF
	#define TMC2209_INTPOL              1   // Step interpolation: 0 = off, 1 = on
	#define TMC2209_TOFF                3   // Off time: 1 - 15, 0 = MOSFET disable
	#define TMC2209_TBL                 1   // Blanking time: 0 = 16, 1 = 24, 2 = 36, 3 = 54 clocks
	#define TMC2209_HSTRT               1   // Hysteresis start: 1 - 8
	#define TMC2209_HEND               -1   // Hysteresis end: -3 - 12
	#define TMC2209_HMAX               16   // HSTRT + HEND

	#define TMC2209_IHOLDDELAY  10 // 0 - 15

	// TPOWERDOWN
	#define TMC2209_TPOWERDOWN          20 // 0 - ((2^8)-1) * 2^18 tCLK

	// TPWMTHRS
	#define TMC2209_TPWM_THRS           TMC_THRESHOLD_MIN   // tpwmthrs: 0 - 2^20 - 1 (20 bits)

	// PWMCONF - StealthChop defaults
	#define TMC2209_PWM_FREQ            1   // 0 = 1/1024, 1 = 2/683, 2 = 2/512, 3 = 2/410 fCLK
	#define TMC2209_PWM_AUTOGRAD        1   // boolean (0 or 1)
	#define TMC2209_PWM_GRAD            14  // 0 - 255
	#define TMC2209_PWM_LIM             12  // 0 - 15
	#define TMC2209_PWM_REG             8   // 1 - 15
	#define TMC2209_PWM_OFS             36  // 0 - 255

	// TCOOLTHRS
	#define TMC2209_COOLSTEP_THRS       TMC_THRESHOLD_MIN   // tpwmthrs: 0 - 2^20 - 1 (20 bits)

	// COOLCONF - CoolStep defaults
	#define TMC2209_SEMIN               5   // 0 = coolStep off, 1 - 15 = coolStep on
	#define TMC2209_SEUP                0   // 0 - 3 (1 - 8)
	#define TMC2209_SEMAX               2   // 0 - 15
	#define TMC2209_SEDN                1   // 0 - 3
	#define TMC2209_SEIMIN              0   // boolean (0 or 1)

	// end of default values

	#if TMC2209_MODE == 0   // StealthChop
	#define TMC2209_PWM_AUTOSCALE 1
	#define TMC2209_SPREADCYCLE   0
	#elif TMC2209_MODE == 1 // CoolStep
	#define TMC2209_PWM_AUTOSCALE 0
	#define TMC2209_SPREADCYCLE   1
	#else                   //StallGuard
	#define TMC2209_PWM_AUTOSCALE 0
	#define TMC2209_SPREADCYCLE   0
	#endif

	typedef enum {
		TMC2209_Microsteps_1 = 1,
		TMC2209_Microsteps_2 = 2,
		TMC2209_Microsteps_4 = 4,
		TMC2209_Microsteps_8 = 8,
		TMC2209_Microsteps_16 = 16,
		TMC2209_Microsteps_32 = 32,
		TMC2209_Microsteps_64 = 64,
		TMC2209_Microsteps_128 = 128,
		TMC2209_Microsteps_256 = 256
	} tmc2209_microsteps_t;

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

	typedef union {
	    uint8_t data[8];
	    struct {
	        uint8_t sync;
	        uint8_t slave;
	        uint8_t addr;
	        uint32_t payload;
	        uint8_t crc;
	    } msg;
	} TMC_uart_write_datagram_t;

	typedef union {
	    uint8_t data[4];
	    struct {
	        uint8_t sync;
	        uint8_t slave;
	        uint8_t addr;
	        uint8_t crc;
	    } msg;
	} TMC_uart_read_datagram_t;

	HAL_StatusTypeDef DRV_ReadRegister(uint8_t, uint32_t*);
	HAL_StatusTypeDef DRV_WriteRegister(uint8_t, uint32_t);
	void tmc2209_init(void);

#endif /* INC_DRIVER_H_ */
