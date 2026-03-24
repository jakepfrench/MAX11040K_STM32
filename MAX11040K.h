/*
 * MAX11040K.h
 *
 *  Created on: Jan 7, 2026
 *      Author: J. French
 */


#ifndef __MAX11040K_H
#define __MAX11040K_H

#include "main.h"


// Use SPI1, these are external
extern SPI_HandleTypeDef hspi1;


/* From table 2. Command Bytes
+-----------+----------+------+--------------+-----------------------------------+
| R/W       | ADDRESS  | HEX  | DATA LENGTH  | FUNCTION                          |
| [A7]      | [A6:A0]  |      |              |                                   |
+-----------+----------+------+--------------+-----------------------------------+
| Write  0  | 100 0000 | 0x40 | 32 × n bits  | Sampling Instant Control Register |	01000000	0x40
| Read   1  | 100 0000 | 0xC0 | 32 × n bits  | Sampling Instant Control Register |  11000000	0xC0
| Write  0  | 101 0000 | 0x50 | 16 bits      | Data-Rate Control Register        |  01010000	0x50
| Read   1  | 101 0000 | 0xD0 | 16 bits      | Data-Rate Control Register        |  11010000	0xD0
| Write  0  | 110 0000 | 0x60 | 8 × n bits   | Configuration Register            |  01100000	0x60
| Read   1  | 110 0000 | 0xE0 | 8 × n bits   | Configuration Register            |  11100000	0xE0
| Read   1  | 111 0000 | 0xF0 | 96 × n bits  | Data Register                     |  11110000	0xF0
+-----------+----------+------+--------------+-----------------------------------+
*/
// define registers
typedef enum
{
    REG_SAMPLING_INSTANT_WRITE  = 0x40,
    REG_SAMPLING_INSTANT_READ	= 0xC0,

    REG_DATA_RATE_CONTROL_WRITE = 0x50,
    REG_DATA_RATE_CONTROL_READ  = 0xD0,

    REG_CONFIGURATION_WRITE     = 0x60,
    REG_CONFIGURATION_READ      = 0xE0,

    REG_DATA_READ               = 0xF0
} MAX11040K_reg_t;

typedef enum {
    MAX11040K_CH0 = 0,
    MAX11040K_CH1 = 3,
    MAX11040K_CH2 = 6,
    MAX11040K_CH3 = 9,
} MAX11040K_Channel_t;


// static inline functions
static inline void MAX11040K_chip_select(void)
{
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
}

static inline void MAX11040K_chip_deselect(void)
{
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}
//
//static inline uint8_t MAX11040K_DRDY(void)
//{
//    return (HAL_GPIO_ReadPin(DRDYOUT_GPIO_Port, DRDYOUT_Pin) == GPIO_PIN_RESET);
//}


static inline int32_t MAX11040K_value(const uint8_t *buf, MAX11040K_Channel_t ch)
{
    int32_t val =
        ((int32_t)buf[ch]     << 16) |
        ((int32_t)buf[ch + 1] << 8)  |
        ((int32_t)buf[ch + 2]);

   if (val & 0x800000)
        val |= 0xFF000000;
   //val -= 26500;
    return val;
}


static inline float MAX11040K_to_voltage(int32_t code)
{
    const float VREF = 2.5f;
    const float FS = 0.88f * VREF;   // 2.2 V
    return ((float)code / 8388608.0f) * FS;  //2^24 / 2
}




// Function prototypes
uint8_t MAX11040K_bytes_size(MAX11040K_reg_t reg);
void MAX11040K_write_reg(MAX11040K_reg_t reg, uint8_t *payload);
void MAX11040K_read_reg(MAX11040K_reg_t reg, uint8_t *payload);
void MAX11040K_reset();


#endif
