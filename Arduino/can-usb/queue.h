#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>

#define BUF_SIZE_RX 256 

struct Queue {
  uint8_t buffer[BUF_SIZE_RX];
  uint8_t in;
  uint8_t out;
};

void writeToQueue(struct Queue *q, uint8_t value);
uint8_t readFromQueue(struct Queue *q);
uint8_t isQueueReady(struct Queue *q);

#endif /* QUEUE_H_ */
