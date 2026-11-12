#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "buffer.h"

// initialize buffer depending on data type
void rb_init(ring_buffer *rb, void *storage, size_t element_size, size_t max) {
    rb->buffer = storage;
    rb->element_size = element_size;
    rb->max = max;
    rb->head = 0;
    rb->tail = 0;
}

// used by find_telemetry
int push_discard_newest(ring_buffer *rb, const void *data) {
    size_t next;
    next = rb->head + 1;

    if (next >= rb->max) next = 0; // circle back if max hit

    // if head + 1 = tail -> full buffer -> reject new/incoming value
    if (next == rb->tail) return -1; 

    memcpy(&rb->buffer[rb->head * rb->element_size], data, rb->element_size);
    rb->head = next;
    return 0;
}

// used by sensor update
int push_overwrite_oldest(ring_buffer *rb, const void *data) {
    size_t next;
    next = rb->head + 1;

    if (next >= rb->max) next = 0; // circle back if max hit

    // if head + 1 = tail -> full buffer -> overwrite old by moving tail (where oldest is)
    if (next == rb->tail) {
        rb->tail++;
        if (rb->tail >= rb->max) rb->tail = 0;
    }

    memcpy(&rb->buffer[rb->head * rb->element_size], data, rb->element_size);
    rb->head = next;
    return 0;
}

int pop(ring_buffer *rb, void *out) {
    size_t next;

    // check if empty
    if (rb->head == rb->tail) return -1;

    next = rb->tail + 1;

    // if next in tail reaches max, circle back to 0
    if (next >= rb->max) next = 0;

    // pop from buffer and read into output
    // memcpy to make generic buffer pop function (usable for both sensor and telemetry)
    // memcpy: addr of destination, src, size
    memcpy(out, &rb->buffer[rb->tail * rb->element_size], rb->element_size);
    rb->tail = next;
    return 0;
}