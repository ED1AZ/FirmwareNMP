#pragma once
#include <stdint.h>
#include "stm32f4xx_hal.h"
// assume ALT ADDRESS HIGH (Pin 12 on accelerometer is HIGH)
// so, ADXL343 7-bit I2C address is 0x1D
// use bitwise left shift to make room for R/W flag using LSB
#define ADXL343_I2C_ADDR (0x1D << 1)

// register maps
#define ADXL343_REG_DEVID 0x00
#define ADXL343_REG_DATA_FORMAT 0x31
#define ADXL343_REG_DATAX0 0x32
#define ADXL343_REG_DATAX1 0x33
#define ADXL343_REG_DATAY0 0x34
#define ADXL343_REG_DATAY1 0x35
#define ADXL343_REG_DATAZ0 0x36
#define ADXL343_REG_DATAZ1 0x37
#define ADXL343_REG_POWER_CTL 0x2D
#define ADXL343_REG_DATA_RATE 0x2C

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} adxl343_axes;

HAL_StatusTypeDef adxl343_init(void);
HAL_StatusTypeDef adxl343_read_axes(adxl343_axes *axes);
