#ifndef CAN_H_
#define CAN_H_

#include <stdint.h>
struct CanMsg{
  uint16_t id;                    // Frame ID
  uint8_t len;                    // Data Length
  uint8_t dataByte[8];            // Data Bytes
};    

uint8_t transmitCan();

#endif /* CAN_H_ */
