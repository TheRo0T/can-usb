#include "queue.h"

Queue::Queue() {
  in = 0;
  out = 0;
}

void Queue::write(uint8_t value) {
  buffer[in++] = value;
}

uint8_t Queue::read() {
  return buffer[out++];
}

uint8_t Queue::isReady() {
  return in != out;
}
