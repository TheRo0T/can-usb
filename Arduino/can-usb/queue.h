#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>

#define BUF_SIZE_RX 256 

class Queue {
  private:
    uint8_t buffer[BUF_SIZE_RX];
    uint8_t in;
    uint8_t out;
  public:
    Queue();
    void write(uint8_t value);
    uint8_t read();
    uint8_t isReady();
};

#endif /* QUEUE_H_ */
