#pragma once
#include <stdint.h>
#include "stm32f4xx_hal.h"

// GPIO pins/ports
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define BUTTON_Pin GPIO_PIN_0
#define BUTTON_GPIO_Port GPIOA

#define ACCEL_THRESHOLD_G 1.5f // user-specified magnitude in G's
#define ADXL343_G_PER_LSB 0.0039f // full-resolution mode, so 1 LSB = 0.0039 g (1/256 g)
#define BUF_LEN 16
#define DEBOUNCE_MS 50

// task structure
typedef struct {
    void (*run)(void);
    uint32_t interval_ms;
    uint32_t last_ms;
} task_s;

void run_scheduler(void);
void tasks_init(I2C_HandleTypeDef *hi2c);