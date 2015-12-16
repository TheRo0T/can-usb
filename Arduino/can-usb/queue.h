#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>

template <int SIZE, uint8_t MASK, typename EntryType>
class Queue {
  private:
    EntryType buffer[SIZE];
    uint8_t in;
    uint8_t out;
    
  public:
    Queue() {
      in = 0;
      out = 0;
    }
    
    void write(EntryType value) {
      buffer[in++] = value;
      in &= MASK;
    }
    
    EntryType read() {
      EntryType result = buffer[out++];
      out &= MASK;
      return result;
    }
    
    uint8_t isReady() {
      return in != out;
    }
};

#endif /* QUEUE_H_ */
