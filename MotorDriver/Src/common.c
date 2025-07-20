#include "../Inc/common.h"

extern UART_HandleTypeDef huart1;
static uint8_t tmc_motors = 0;

// 1 - 256 in steps of 2^value is valid
bool tmc_microsteps_validate (uint16_t microsteps)
{
    uint_fast8_t i = 8, count = 0;

    if(microsteps <= 256) do {
        if(microsteps & 0x01)
            count++;
        microsteps >>= 1;
    } while(i--);

    return count == 1;
}

uint8_t tmc_microsteps_to_mres (uint16_t microsteps)
{
    uint8_t value = 0;

    microsteps = microsteps == 0 ? 1 : microsteps;

    while((microsteps & 0x01) == 0) {
        value++;
        microsteps >>= 1;
    }

    return 8 - (value > 8 ? 8 : value);
}

uint32_t tmc_calc_tstep (trinamic_config_t *config, float mm_sec, float steps_mm)
{
    uint32_t den = (uint32_t)(256.0f * mm_sec * steps_mm);

    return den ? (config->microsteps * config->f_clk) / den : 0;
}

float tmc_calc_tstep_inv (trinamic_config_t *config, uint32_t tstep, float steps_mm)
{
    return tstep == 0 ? 0.0f : (float)(config->f_clk * config->microsteps) / (256.0f * (float)tstep * steps_mm);
}

void tmc_motors_set (uint8_t motors)
{
    tmc_motors = motors;
}

uint8_t tmc_motors_get (void)
{
    return tmc_motors;
}

void tmc_crc8 (uint8_t *datagram, uint8_t datagramLength)
{
    int i,j;
    uint8_t *crc = datagram + (datagramLength - 1); // CRC located in last byte of message
    uint8_t currentByte;
    *crc = 0;
    for (i = 0; i < (datagramLength - 1); i++) {    // Execute for all bytes of a message
        currentByte = datagram[i];                  // Retrieve a byte to be sent from Array
        for (j = 0; j < 8; j++) {
            if ((*crc >> 7) ^ (currentByte & 0x01)) // update CRC based result of XOR operation
                *crc = (*crc << 1) ^ 0x07;
            else
                *crc = (*crc << 1);
            currentByte = currentByte >> 1;
        } // for CRC bit
    }
}

void tmc_uart_write (trinamic_motor_t driver, TMC_uart_write_datagram_t *datagram)
{
	HAL_HalfDuplex_EnableTransmitter(&huart1);
	HAL_UART_Transmit(&huart1, datagram->data, sizeof(TMC_uart_write_datagram_t), HAL_MAX_DELAY);
}

TMC_uart_write_datagram_t *tmc_uart_read (trinamic_motor_t driver, TMC_uart_read_datagram_t *datagram)
{
	static TMC_uart_write_datagram_t res;

	HAL_HalfDuplex_EnableTransmitter(&huart1);
	HAL_StatusTypeDef wstatus = HAL_UART_Transmit(&huart1, datagram->data, sizeof(TMC_uart_read_datagram_t), HAL_MAX_DELAY);
	if(wstatus == HAL_OK){
		uint8_t buffer[8] = {0};
		for(int i = 0; i < sizeof(buffer) ; i++){
			HAL_HalfDuplex_EnableReceiver(&huart1);
			HAL_UART_Receive(&huart1, &buffer[i], sizeof(buffer), 100);
		}

		memcpy(res.data, buffer, sizeof(buffer));
		uint8_t crc = res.msg.crc;
		tmc_crc8(res.data, sizeof(TMC_uart_write_datagram_t));
		if(res.msg.slave == 0xFF && crc == res.msg.crc){
			return &res;
		}
	}
	return 0;
}
