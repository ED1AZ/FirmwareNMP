#include "scheduler.h"
#include "../drivers/adxl343.h"
#include "../buffer/buffer.h"

// define drivers + ring buffers
static ADXL343_t accel;
static uint8_t sensor_enabled = 0;
static ADXL343_t sample_storage[BUF_LEN];
static ring_buffer sample_rb;
static ADXL343_t vector_storage[BUF_LEN];
static ring_buffer vector_rb;

static void task_sample(void) {
    // sample sensor
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