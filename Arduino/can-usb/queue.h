#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>

template <int SIZE, typename EntryType>
class Queue {
  private:
    EntryType buffer[SIZE];
    uint8_t in;
    uint8_t out;
    uint8_t mask;
    
  public:
    Queue() {
      in = 0;
      out = 0;
      mask = SIZE - 1;
    }
    
    void write(EntryType value) {
      buffer[in++] = value;
      in &= mask;
    }
    
    EntryType read() {
      EntryType result = buffer[out++];
      out &= mask;
      return result;
    }
    
    uint8_t isReady() {
      return in != out;
    }
};

#endif /* QUEUE_H_ */