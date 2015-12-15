#include "queue.h"

void writeToQueue(struct Queue *q, uint8_t value) {
  q->buffer[q->in++] = value;
}

uint8_t readFromQueue(struct Queue *q) {
  return q->buffer[q->out++];
}

uint8_t isQueueReady(struct Queue *q) {
  return q->in != q->out;
}
