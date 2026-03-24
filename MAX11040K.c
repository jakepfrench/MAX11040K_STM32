/*
 * MAX11040K.c
 *
 *  Created on: Jan 7, 2026
 *      Author: J. French
 */


#include "MAX11040K.h"
#include <string.h>

// Map registers to byte size
// Send a byte to the command register
uint8_t MAX11040K_bytes_size(MAX11040K_reg_t reg)
{
	uint8_t byteSize = 0;
	switch (reg) {
		case REG_SAMPLING_INSTANT_WRITE:
		case REG_SAMPLING_INSTANT_READ:
			byteSize = 4;
		break;

		case REG_DATA_RATE_CONTROL_WRITE:
		case REG_DATA_RATE_CONTROL_READ:
			byteSize = 2;
		break;

		case REG_CONFIGURATION_WRITE:
		case REG_CONFIGURATION_READ:
			byteSize = 1;
		break;

		case REG_DATA_READ:
			byteSize = 12;
		break;

		default:
			byteSize = 0;
	}
	return byteSize;
}


void MAX11040K_write_reg(MAX11040K_reg_t reg, uint8_t *payload)
{
    uint8_t tx[20];
    uint8_t payload_len = MAX11040K_bytes_size(reg);

    tx[0] = reg;
    for (int i = 0; i < payload_len; i++)
    	tx[1 + i] = payload[i];

    MAX11040K_chip_select();
    for (volatile int i = 0; i < 50; i++)
    	__ASM volatile ("NOP");
    HAL_SPI_Transmit(&hspi1, tx, payload_len + 1, HAL_MAX_DELAY);
    //for (volatile int i = 0; i < 100; i++)
    //	__ASM volatile ("NOP");
    MAX11040K_chip_deselect();
    HAL_Delay(50);
}

#define MAX11040K_RESET_GPIO_Port GPIOB    // Replace with actual port
#define MAX11040K_RESET_Pin       GPIO_PIN_0 // Replace with actual pin

void MAX11040K_reset(void)
{
    // Bring RESET high
    HAL_GPIO_WritePin(MAX11040K_RESET_GPIO_Port, MAX11040K_RESET_Pin, GPIO_PIN_SET);

    // Hold for at least 1 µs (safe: 10 µs)
    for (volatile int i = 0; i < 100; i++)
        __ASM volatile ("NOP");

    // Bring RESET low
    HAL_GPIO_WritePin(MAX11040K_RESET_GPIO_Port, MAX11040K_RESET_Pin, GPIO_PIN_RESET);

    // Wait a few ms for internal reference & clock to stabilize
    HAL_Delay(1000);
}



void MAX11040K_read_reg(MAX11040K_reg_t reg, uint8_t *payload)
{
    uint8_t tx[13] = {0};
    uint8_t rx[13] = {0};

    tx[0] = reg;
    MAX11040K_chip_select();
    HAL_SPI_TransmitReceive(&hspi1, tx, rx, 13, 0);
    MAX11040K_chip_deselect();

    memcpy(payload, &rx[1], 12);
}
