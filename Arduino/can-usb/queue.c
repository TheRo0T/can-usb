#include "queue.h"

void writeToQueue(struct queue *q, uint8_t value) {
    q->buffer[q->in++] = value;
}

uint8_t readFromQueue(struct queue *q) {
    return q->buffer[q->out++];
}

uint8_t isQueueReady(struct queue *q) {
    return q->in != q->out;
}
