#include "scheduler.h"
#include "../drivers/adxl343.h"
#include "../buffer/buffer.h"

// define drivers + ring buffers
static ADXL343_t accel;
static uint8_t sensor_enabled = 0;
static adxl343_axes sample_storage[BUF_LEN];
static ring_buffer sample_rb;
static adxl343_axes vector_storage[BUF_LEN];
static ring_buffer vector_rb;

static void task_sample(void) {
    uint8_t last_pressed = 0;
    uint32_t last_change = 0;

    uint8_t pressed = (HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin) == GPIO_PIN_RESET);
    uint32_t now = HAL_GetTick();

    // button to toggle sensor enabled state (toggle on press, with debounce)
    if (pressed && !last_pressed && (now - last_change) >= DEBOUNCE_MS) {
        sensor_enabled = !sensor_enabled;

        // enable or disable the sensor
        if (sensor_enabled) adxl343_enable(&accel);
        else adxl343_disable(&accel);
        
        last_change = now;
    }
    last_pressed = pressed;

    if (sensor_enabled) {
        adxl343_axes sample;
        // read acceleration data from the sensor
        adxl343_read_axes(&accel, &sample);
        // store the sample in the sample ring buffer
        push_overwrite_oldest(&sample_rb, &sample);
    }
}

static void task_check(void) {
    // check sensor status
}

static void task_print(void) {
    // print acceleration vector
}

static task_s tasks[] = {
    { task_sample, 10,  0 },   // 100 Hz, matches sensor output rate 
    { task_check,  10,  0 },
    { task_print,  200, 0 },   // 5 Hz so the serial output is readable
};

#define NUM_TASKS (sizeof(tasks) / sizeof(tasks[0]))

void run_scheduler(void) {
    uint32_t now = HAL_GetTick();
 
    for (unsigned i = 0; i < NUM_TASKS; i++) {
        if ((uint32_t)(now - tasks[i].last_ms) >= tasks[i].interval_ms) {
            tasks[i].last_ms = now;
            tasks[i].run();
        }
    }
}

void tasks_init(I2C_HandleTypeDef *hi2c) {
    adxl343_init(&accel, hi2c);
    rb_init(&sample_rb, sample_storage, sizeof(adxl343_axes), BUF_LEN);
    rb_init(&vector_rb, vector_storage, sizeof(adxl343_axes), BUF_LEN);
    sensor_enabled = 1;
}