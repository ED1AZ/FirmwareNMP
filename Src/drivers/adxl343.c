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