#include "adxl343.h"

HAL_StatusTypeDef adxl343_init(ADXL343_t *dev, I2C_HandleTypeDef *hi2c) {
    // set I2C handle for device
    dev->hi2c = hi2c;

    // check device status and id
    uint8_t device_id;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(dev->hi2c, ADXL343_I2C_ADDR, ADXL343_REG_DEVID, I2C_MEMADD_SIZE_8BIT, &device_id, 1, HAL_MAX_DELAY);
    if (status != HAL_OK || device_id != 0xE5) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef adxl343_enable(ADXL343_t *dev) {
    // set D3 bit of POWER_CTL register to enable measurement mode
    uint8_t power_ctl;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(dev->hi2c, ADXL343_I2C_ADDR, ADXL343_REG_POWER_CTL, I2C_MEMADD_SIZE_8BIT, &power_ctl, 1, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return HAL_ERROR;
    }
    
    // bit shift to set D3 bit to 1
    // 1 = 00000001, then 3 shifts it to 00001000, then OR with current register to set D3=1
    power_ctl |= (1 << 3);
    return HAL_I2C_Mem_Write(dev->hi2c, ADXL343_I2C_ADDR, ADXL343_REG_POWER_CTL, I2C_MEMADD_SIZE_8BIT, &power_ctl, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef adxl343_disable(ADXL343_t *dev) {
    // clear D3 bit of POWER_CTL register to disable measurement mode
    uint8_t power_ctl;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(dev->hi2c, ADXL343_I2C_ADDR, ADXL343_REG_POWER_CTL, I2C_MEMADD_SIZE_8BIT, &power_ctl, 1, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return HAL_ERROR;
    }

    // bit shift to clear D3 bit to 0
    // 1 = 00000001, then 3 shifts it to 00001000, then AND NOT with current register to clear D3=0
    power_ctl &= ~(1 << 3);
    return HAL_I2C_Mem_Write(dev->hi2c, ADXL343_I2C_ADDR, ADXL343_REG_POWER_CTL, I2C_MEMADD_SIZE_8BIT, &power_ctl, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef adxl343_read_axes(ADXL343_t *dev, adxl343_axes *axes) {
    // read 6 bytes, starting from DATAX0 register
    uint8_t data[6];
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(dev->hi2c, ADXL343_I2C_ADDR, ADXL343_REG_DATAX0, I2C_MEMADD_SIZE_8BIT, data, 6, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return HAL_ERROR;
    }
    // shift high byte to combine with low byte to form 16-bit signed integer
    axes->x = (int16_t)((data[1] << 8) | data[0]);
    axes->y = (int16_t)((data[3] << 8) | data[2]);
    axes->z = (int16_t)((data[5] << 8) | data[4]);

    return HAL_OK;
}