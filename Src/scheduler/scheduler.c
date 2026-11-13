#include "scheduler.h"
#include "../drivers/adxl343.h"
#include "../buffer/buffer.h"
#include <math.h>

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
    // check the acceleration magnitude against the threshold
    adxl343_axes sample;
    while (pop(&sample_rb, &sample) == 0) {
        // magnitude of the acceleration vector = sqrt(x^2 + y^2 + z^2), then turn into Gs
        float magnitude = sqrtf(sample.x * sample.x + sample.y * sample.y + sample.z * sample.z) * ADXL343_G_PER_LSB;
        // store the sample in the vector ring buffer
        push_overwrite_oldest(&vector_rb, &sample);

        // threshold check for active low LED: RESET = on and SET = off
        if (magnitude >= ACCEL_THRESHOLD_G) 
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        else
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        
    }
}

static void task_print(void) {
    // print current acceleration vector, which is newest in vector ring buffer (at head)
    adxl343_axes sample, latest = {0, 0, 0};
    uint8_t has_sample = 0;
    // pop function reads from oldest, so read all samples to get the newest
    while (pop(&vector_rb, &sample) == 0) {
        latest = sample;
        has_sample = 1;
    }
    if (has_sample) {
        printf("Acceleration vector: x=%.3f, y=%.3f, z=%.3f\n",
                latest.x * ADXL343_G_PER_LSB, 
                latest.y * ADXL343_G_PER_LSB, 
                latest.z * ADXL343_G_PER_LSB);
    }
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