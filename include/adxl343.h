#pragma once
#include <stdint.h>
// assume ALT ADDRESS HIGH (Pin 12 on accelerometer is HIGH)
// so, ADXL343 7-bit I2C address is 0x1D
// use bitwise left shift to make room for R/W flag using LSB
#define ADXL343_I2C_ADDR (0x1D << 1)

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} adxl343_axes;

