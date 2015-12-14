#ifndef QUEUE_H_
#define QUEUE_H_

#define BUF_SIZE_RX 256 

struct queue {
    uint8_t buffer[BUF_SIZE_RX];
    uint8_t in;
    uint8_t out;
};

void writeToQueue(struct queue *q, uint8_t value);
uint8_t readFromQueue(struct queue *q);
uint8_t isQueueReady(struct queue *q);

#endif /* QUEUE_H_ */
