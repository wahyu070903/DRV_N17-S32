/*
 * AS5600.c
 *
 *  Created on: Mar 1, 2025
 *      Author: Orsted
 */


#include "../Inc/AS5600.h"


AS5600_TypeDef *AS5600_New(void) {
    AS5600_TypeDef *a = (AS5600_TypeDef *)calloc(1, sizeof(AS5600_TypeDef));
    return a;
}

AS5600Init_StatusTypedef AS5600_Init(AS5600_TypeDef *a) {
    HAL_StatusTypeDef status = AS5600_INIT_OK;
    uint8_t pwm = 0;
    uint8_t mag_status = 0;
    /* Set configuration defaults for uninitialized values. */
    if (!(a->PositiveRotationDirection)) {
        a->PositiveRotationDirection = AS5600_DIR_CW;
    }
    if (!(a->LowPowerMode)) {
        a->LowPowerMode = AS5600_POWER_MODE_DEFAULT;
    }
    if (!(a->Hysteresis)) {
        a->Hysteresis = AS5600_HYSTERESIS_DEFAULT;
    }
    if (!(a->OutputMode)) {
        a->OutputMode = AS5600_OUTPUT_STAGE_DEFAULT;
    }
    if (!(a->PWMFrequency)) {
        a->PWMFrequency = AS5600_PWM_FREQUENCY_DEFAULT;
    }
    if (!(a->SlowFilter)) {
        a->SlowFilter = AS5600_SLOW_FILTER_DEFAULT;
    }
    if (!(a->FastFilterThreshold)) {
        a->FastFilterThreshold = AS5600_FAST_FILTER_DEFAULT;
    }
    if (!(a->WatchdogTimer)) {
        a->WatchdogTimer = AS5600_WATCHDOG_DEFAULT;
    }
    /* Write configuration settings.
       Do this in single write instead of using functions below to avoid
       overhead of multiple calls to HAL_I2C_Mem_Write_IT */
    switch (a->LowPowerMode) {
        case AS5600_POWER_MODE_NOM:
            a->confRegister[1] &= ~((1UL << 1) | (1UL << 0));
            break;
        case AS5600_POWER_MODE_LPM1:
            a->confRegister[1] |= (1UL << 0);
            a->confRegister[1] &= ~(1UL << 1);
            break;
        case AS5600_POWER_MODE_LPM2:
            a->confRegister[1] |= (1UL << 1);
            a->confRegister[1] &= (1UL << 0);
            break;
        case AS5600_POWER_MODE_LPM3:
            a->confRegister[1] |= ((1UL << 1) | (1UL << 0));
            break;
        default:
            /* Invalid low power mode specified */
            status = AS5600_INIT_ERR;
            return status;
    }
    switch (a->Hysteresis) {
        case AS5600_HYSTERESIS_OFF:
            a->confRegister[1] &= ~((1UL << 3) | (1UL << 2));
            break;
        case AS5600_HYSTERESIS_1LSB:
            a->confRegister[1] |= (1UL << 2);
            a->confRegister[1] &= ~(1UL << 3);
            break;
        case AS5600_HYSTERESIS_2LSB:
            a->confRegister[1] &= ~(1UL << 2);
            a->confRegister[1] |= (1UL << 3);
            break;
        case AS5600_HYSTERESIS_3LSB:
            a->confRegister[1] |= ((1UL << 3) | (1UL << 2));
            break;
        default:
            /* Invalid hysteresis mode specified */
            status = AS5600_INIT_ERR;
            return status;
    }
    switch (a->OutputMode) {
        case AS5600_OUTPUT_STAGE_FULL:
            a->confRegister[1] &= ~((1UL << 5) | (1UL << 4));
            break;
        case AS5600_OUTPUT_STAGE_REDUCED:
            a->confRegister[1] |= (1UL << 4);
            a->confRegister[1] &= ~(1UL << 5);
            break;
        case AS5600_OUTPUT_STAGE_PWM:
            a->confRegister[1] &= (1UL << 4);
            a->confRegister[1] |= (1UL << 5);
            pwm = 1;
            break;
        default:
            /* Invalid output mode specified */
            status = AS5600_INIT_ERR;
            return status;
    }
    if (pwm) {
        switch (a->PWMFrequency) {
            case AS5600_PWM_FREQUENCY_115HZ:
                a->confRegister[1] &= ~((1UL << 7) | (1UL << 6));
                break;
            case AS5600_PWM_FREQUENCY_230HZ:
                a->confRegister[1] |= (1UL << 6);
                a->confRegister[1] &= ~(1UL << 7);
                break;
            case AS5600_PWM_FREQUENCY_460HZ:
                a->confRegister[1] &= ~(1UL << 6);
                a->confRegister[1] |= (1UL << 7);
                break;
            case AS5600_PWM_FREQUENCY_920HZ:
                a->confRegister[1] |= ((1UL << 7) | (1UL << 6));
                break;
            default:
                /* Invalid PWM frequency specified. */
                status = AS5600_INIT_ERR;
                return status;
        }
    }
    switch (a->SlowFilter) {
        case AS5600_SLOW_FILTER_16X:
            a->confRegister[0] &= ~((1UL << 1) | (1UL << 0));
            break;
        case AS5600_SLOW_FILTER_8X:
            a->confRegister[0] |= (1UL << 0);
            a->confRegister[0] &= ~(1UL << 1);
            break;
        case AS5600_SLOW_FILTER_4X:
            a->confRegister[0] &= ~(1UL << 0);
            a->confRegister[0] |= (1UL << 1);
            break;
        case AS5600_SLOW_FILTER_2X:
            a->confRegister[0] |= ((1UL << 1) | (1UL << 0));
            break;
        default:
            /* Invalid slow filter mode specified */
            status = AS5600_INIT_ERR;
            return status;
    }
    switch (a->FastFilterThreshold) {
        case AS5600_FAST_FILTER_SLOW_ONLY:
            a->confRegister[0] &= ~((1UL << 4) | (1UL << 3) | (1UL << 2));
            break;
        case AS5600_FAST_FILTER_6LSB:
            a->confRegister[0] &= ~((1UL << 4) | (1UL << 3));
            a->confRegister[0] |= (1UL << 2);
            break;
        case AS5600_FAST_FILTER_7LSB:
            a->confRegister[0] &= ~((1UL << 4) | (1UL << 2));
            a->confRegister[0] |= (1UL << 3);
            break;
        case AS5600_FAST_FILTER_9LSB:
            a->confRegister[0] &= ~(1UL << 4);
            a->confRegister[0] |= ((1UL << 3) | (1UL << 2));
            break;
        case AS5600_FAST_FILTER_18LSB:
            a->confRegister[0] &= ~((1UL << 3) | (1UL << 2));
            a->confRegister[0] |= (1UL << 4);
            break;
        case AS5600_FAST_FILTER_21LSB:
            a->confRegister[0] &= ~(1UL << 3);
            a->confRegister[0] |= ((1UL << 4) | (1UL << 2));
            break;
        case AS5600_FAST_FILTER_24LSB:
            a->confRegister[0] &= ~(1UL << 2);
            a->confRegister[0] |= ((1UL << 4) | (1UL << 3));
            break;
        case AS5600_FAST_FILTER_10LSB:
            a->confRegister[0] |= ((1UL << 4) | (1UL << 3) | (1UL << 2));
            break;
        default:
            /* Invalid slow filter mode specified */
            status = AS5600_INIT_ERR;
            return status;
    }
    switch (a->WatchdogTimer) {
        case AS5600_WATCHDOG_OFF:
            a->confRegister[0] &= ~(1UL << 6);
            break;
        case AS5600_WATCHDOG_ON:
            a->confRegister[0] |= (1UL << 6);
            break;
        default:
            /* Invalid watchdog state specified */
            status = AS5600_INIT_ERR;
            return status;
    }
    if (HAL_I2C_Mem_Write(a->i2cHandle, a->i2cAddr, AS5600_REGISTER_CONF_HIGH, I2C_MEMADD_SIZE_8BIT, a->confRegister, 2, 10) != HAL_OK) {
        status = AS5600_INIT_HAL_FAIL;
        return status;
    }
    /* Check magnet status */
    if (AS5600_GetMagnetStatus(a, &mag_status) != HAL_OK) {
        status = AS5600_INIT_HAL_FAIL;
        return status;
    }
    if (!(mag_status & AS5600_MAGNET_DETECTED)) {
        /* Magnet not detected */
        status = AS5600_INIT_MAG_NOT;
        return status;
    }
    if ((mag_status & AS5600_AGC_MIN_GAIN_OVERFLOW)) {
        /* B-field is too strong */
        status = AS5600_INIT_MAG_STG;
        return status;
    }
    if ((mag_status & AS5600_AGC_MAX_GAIN_OVERFLOW)) {
        /* B-field is too weak */
        status = AS5600_INIT_MAG_WEK;
        return status;
    }
    /* Write */

    return status;
}

HAL_StatusTypeDef AS5600_SetStartPosition(AS5600_TypeDef *const a,
                                          const uint16_t pos) {
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t data[2] = {0};
    data[0] = (uint8_t)(
        (pos & AS5600_12_BIT_MASK) >>
        8); /* Zero out upper four bits of argument and shift out lower four
               bits */
    data[1] = (uint8_t)pos;
    if (HAL_I2C_Mem_Write(a->i2cHandle, a->i2cAddr,
                             AS5600_REGISTER_ZPOS_HIGH, I2C_MEMADD_SIZE_8BIT,
                             data, 2, 10) != HAL_OK) {
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef AS5600_SetStopPosition(AS5600_TypeDef *const a,
                                         const uint16_t pos) {
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t data[2] = {0};
    data[0] = (uint8_t)(
        (pos & AS5600_12_BIT_MASK) >>
        8); /* Zero out upper four bits of argument and shift out lower four
               bits */
    data[1] = (uint8_t)pos;
    if (HAL_I2C_Mem_Write(a->i2cHandle, a->i2cAddr,
                             AS5600_REGISTER_MPOS_HIGH, I2C_MEMADD_SIZE_8BIT,
                             data, 2, 10) != HAL_OK) {
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef AS5600_SetMaxAngle(AS5600_TypeDef *const a,
                                     const uint16_t angle) {
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t data[2] = {0};
    data[0] = (uint8_t)((angle & AS5600_12_BIT_MASK) >>
                        8); /* Zero out upper four bits of
                                argument and shift out lower four
                                bits */
    data[1] = (uint8_t)angle;
    if (HAL_I2C_Mem_Write(a->i2cHandle, a->i2cAddr,
                             AS5600_REGISTER_MANG_HIGH, I2C_MEMADD_SIZE_8BIT,
                             data, 2, 10) != HAL_OK) {
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef AS5600_SetPositiveRotationDirection(AS5600_TypeDef *const a, const uint8_t dir) {
    HAL_StatusTypeDef status = HAL_OK;
    if (dir == AS5600_DIR_CW) {
        HAL_GPIO_WritePin(a->DirPort, a->DirPin, GPIO_PIN_RESET);
    } else if (dir == AS5600_DIR_CCW) {
        HAL_GPIO_WritePin(a->DirPort, a->DirPin, GPIO_PIN_SET);
    } else {
        /* Invalid rotation direction specified. */
        status = HAL_ERROR;
    }
    return status;
}

HAL_StatusTypeDef AS5600_SetLowPowerMode(AS5600_TypeDef *const a,
                                         const uint8_t mode) {
    HAL_StatusTypeDef status = HAL_OK;
    switch (mode) {
        case AS5600_POWER_MODE_NOM:
            a->confRegister[1] &= ~((1UL << 1) | (1UL << 0));
            break;
        case AS5600_POWER_MODE_LPM1:
            a->confRegister[1] |= (1UL << 0);
            a->confRegister[1] &= ~(1UL << 1);
            break;
        case AS5600_POWER_MODE_LPM2:
            a->confRegister[1] |= (1UL << 1);
            a->confRegister[1] &= (1UL << 0);
            break;
        case AS5600_POWER_MODE_LPM3:
            a->confRegister[1] |= ((1UL << 1) | (1UL << 0));
            break;
        default:
            /* Invalid low power mode specified */
            status = HAL_ERROR;
            return status;
    }
    if (HAL_I2C_Mem_Write(a->i2cHandle, a->i2cAddr,
                             AS5600_REGISTER_CONF_HIGH, I2C_MEMADD_SIZE_8BIT,
                             a->confRegister, 2, 10) != HAL_OK) {
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef AS5600_SetHysteresis(AS5600_TypeDef *const a,
                                       const uint8_t hysteresis) {
    HAL_StatusTypeDef status = HAL_OK;
    switch (hysteresis) {
        case AS5600_HYSTERESIS_OFF:
            a->confRegister[1] &= ~((1UL << 3) | (1UL << 2));
            break;
        case AS5600_HYSTERESIS_1LSB:
            a->confRegister[1] |= (1UL << 2);
            a->confRegister[1] &= ~(1UL << 3);
            break;
        case AS5600_HYSTERESIS_2LSB:
            a->confRegister[1] &= ~(1UL << 2);
            a->confRegister[1] |= (1UL << 3);
            break;
        case AS5600_HYSTERESIS_3LSB:
            a->confRegister[1] |= ((1UL << 3) | (1UL << 2));
            break;
        default:
            /* Invalid hysteresis mode specified */
            status = HAL_ERROR;
            return status;
    }
    if (HAL_I2C_Mem_Write(a->i2cHandle, a->i2cAddr,
                             AS5600_REGISTER_CONF_HIGH, I2C_MEMADD_SIZE_8BIT,
                             a->confRegister, 2, 10) != HAL_OK) {
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef AS5600_SetOutputMode(AS5600_TypeDef *const a,
                                       const uint8_t mode, uint8_t freq) {
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t pwm = 0;
    switch (mode) {
        case AS5600_OUTPUT_STAGE_FULL:
            a->confRegister[1] &= ~((1UL << 5) | (1UL << 4));
            break;
        case AS5600_OUTPUT_STAGE_REDUCED:
            a->confRegister[1] |= (1UL << 4);
            a->confRegister[1] &= ~(1UL << 5);
            break;
        case AS5600_OUTPUT_STAGE_PWM:
            a->confRegister[1] &= (1UL << 4);
            a->confRegister[1] |= (1UL << 5);
            pwm = 1;
            break;
        default:
            /* Invalid output mode specified */
            status = HAL_ERROR;
            return status;
    }
    if (pwm) {
        switch (freq) {
            case AS5600_PWM_FREQUENCY_115HZ:
                a->confRegister[1] &= ~((1UL << 7) | (1UL << 6));
                break;
            case AS5600_PWM_FREQUENCY_230HZ:
                a->confRegister[1] |= (1UL << 6);
                a->confRegister[1] &= ~(1UL << 7);
                break;
            case AS5600_PWM_FREQUENCY_460HZ:
                a->confRegister[1] &= ~(1UL << 6);
                a->confRegister[1] |= (1UL << 7);
                break;
            case AS5600_PWM_FREQUENCY_920HZ:
                a->confRegister[1] |= ((1UL << 7) | (1UL << 6));
                break;
            default:
                /* Invalid PWM frequency specified. */
                status = HAL_ERROR;
                return status;
        }
    }
    if (HAL_I2C_Mem_Write(a->i2cHandle, a->i2cAddr,
                             AS5600_REGISTER_CONF_HIGH, I2C_MEMADD_SIZE_8BIT,
                             a->confRegister, 2, 10) != HAL_OK) {
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef AS5600_SetSlowFilter(AS5600_TypeDef *const a,
                                       const uint8_t mode) {
    HAL_StatusTypeDef status = HAL_OK;
    switch (mode) {
        case AS5600_SLOW_FILTER_16X:
            a->confRegister[0] &= ~((1UL << 1) | (1UL << 0));
            break;
        case AS5600_SLOW_FILTER_8X:
            a->confRegister[0] |= (1UL << 0);
            a->confRegister[0] &= ~(1UL << 1);
            break;
        case AS5600_SLOW_FILTER_4X:
            a->confRegister[0] &= ~(1UL << 0);
            a->confRegister[0] |= (1UL << 1);
            break;
        case AS5600_SLOW_FILTER_2X:
            a->confRegister[0] |= ((1UL << 1) | (1UL << 0));
            break;
        default:
            /* Invalid slow filter mode specified */
            status = HAL_ERROR;
            return status;
    }
    if (HAL_I2C_Mem_Write(a->i2cHandle, a->i2cAddr,
                             AS5600_REGISTER_CONF_HIGH, I2C_MEMADD_SIZE_8BIT,
                             a->confRegister, 2, 10) != HAL_OK) {
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef AS5600_SetFastFilterThreshold(AS5600_TypeDef *const a,
                                                const uint8_t threshold) {
    HAL_StatusTypeDef status = HAL_OK;
    switch (threshold) {
        case AS5600_FAST_FILTER_SLOW_ONLY:
            a->confRegister[0] &= ~((1UL << 4) | (1UL << 3) | (1UL << 2));
            break;
        case AS5600_FAST_FILTER_6LSB:
            a->confRegister[0] &= ~((1UL << 4) | (1UL << 3));
            a->confRegister[0] |= (1UL << 2);
            break;
        case AS5600_FAST_FILTER_7LSB:
            a->confRegister[0] &= ~((1UL << 4) | (1UL << 2));
            a->confRegister[0] |= (1UL << 3);
            break;
        case AS5600_FAST_FILTER_9LSB:
            a->confRegister[0] &= ~(1UL << 4);
            a->confRegister[0] |= ((1UL << 3) | (1UL << 2));
            break;
        case AS5600_FAST_FILTER_18LSB:
            a->confRegister[0] &= ~((1UL << 3) | (1UL << 2));
            a->confRegister[0] |= (1UL << 4);
            break;
        case AS5600_FAST_FILTER_21LSB:
            a->confRegister[0] &= ~(1UL << 3);
            a->confRegister[0] |= ((1UL << 4) | (1UL << 2));
            break;
        case AS5600_FAST_FILTER_24LSB:
            a->confRegister[0] &= ~(1UL << 2);
            a->confRegister[0] |= ((1UL << 4) | (1UL << 3));
            break;
        case AS5600_FAST_FILTER_10LSB:
            a->confRegister[0] |= ((1UL << 4) | (1UL << 3) | (1UL << 2));
            break;
        default:
            /* Invalid slow filter mode specified */
            status = HAL_ERROR;
            return status;
    }
    if (HAL_I2C_Mem_Write(a->i2cHandle, a->i2cAddr,
                             AS5600_REGISTER_CONF_HIGH, I2C_MEMADD_SIZE_8BIT,
                             a->confRegister, 2, 10) != HAL_OK) {
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef AS5600_SetWatchdogTimer(AS5600_TypeDef *const a,
                                          const uint8_t mode) {
    HAL_StatusTypeDef status = HAL_OK;
    switch (mode) {
        case AS5600_WATCHDOG_OFF:
            a->confRegister[0] &= ~(1UL << 6);
            break;
        case AS5600_WATCHDOG_ON:
            a->confRegister[0] |= (1UL << 6);
            break;
        default:
            /* Invalid watchdog state specified */
            status = HAL_ERROR;
            return status;
    }
    if (HAL_I2C_Mem_Write(a->i2cHandle, a->i2cAddr,
                             AS5600_REGISTER_CONF_HIGH, I2C_MEMADD_SIZE_8BIT,
                             a->confRegister, 2, 10) != HAL_OK) {
        status = HAL_ERROR;
    }

    return status;
}

HAL_StatusTypeDef AS5600_GetRawAngle(AS5600_TypeDef *const a, uint16_t *const angle) {
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t data[2] = {0};
    status = HAL_I2C_Mem_Read(a->i2cHandle, a->i2cAddr, AS5600_REGISTER_RAW_ANGLE_HIGH, I2C_MEMADD_SIZE_8BIT, data, 2, 100);
    if (status != HAL_OK) {
        status = HAL_ERROR;
    }
    *angle = ((data[0] << 8) | data[1]);
    return status;
}

HAL_StatusTypeDef AS5600_GetAngle(AS5600_TypeDef *const a,
                                  uint16_t *const angle) {
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t data[2] = {0};
    if (HAL_I2C_Mem_Read(a->i2cHandle, a->i2cAddr,
                            AS5600_REGISTER_ANGLE_HIGH, I2C_MEMADD_SIZE_8BIT,
                            data, 2, 10) != HAL_OK) {
        status = HAL_ERROR;
    }
    *angle = ((data[0] << 8) | data[1]);

    return status;
}

HAL_StatusTypeDef AS5600_GetMagnetStatus(AS5600_TypeDef *const a, uint8_t *const stat) {
    HAL_StatusTypeDef status = HAL_OK;
    if(HAL_I2C_Mem_Read(a->i2cHandle, a->i2cAddr, AS5600_REGISTER_STATUS,
            I2C_MEMADD_SIZE_8BIT, stat, 1, 10) != HAL_OK){
    	status = HAL_ERROR;
    }
    return status;
}

HAL_StatusTypeDef AS5600_GetAGCSetting(AS5600_TypeDef *const a,
                                       uint8_t *const agc) {
    HAL_StatusTypeDef status = HAL_OK;
    if (HAL_I2C_Mem_Read(a->i2cHandle, a->i2cAddr, AS5600_REGISTER_AGC,
                            I2C_MEMADD_SIZE_8BIT, agc, 1, 10) != HAL_OK) {
        status = HAL_ERROR;
    }
    return status;
}

HAL_StatusTypeDef AS5600_GetCORDICMagnitude(AS5600_TypeDef *const a,
                                            uint16_t *const mag) {
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t data[2] = {0};
    if (HAL_I2C_Mem_Read(a->i2cHandle, a->i2cAddr,
                            AS5600_REGISTER_ANGLE_HIGH, I2C_MEMADD_SIZE_8BIT,
                            data, 2, 10) != HAL_OK) {
        status = HAL_ERROR;
    }
    *mag = ((data[0] << 8) | data[1]);

    return status;
}
