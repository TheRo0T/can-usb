#include <mcp_can.h>
#include <SPI.h>
#include <stdint.h>

uint8_t transmitCan() {
  CAN.sendMsgBuf(canTxMsg.id, 0, canTxMsg.len, canTxMsg.dataByte);    // 0 - standart message
  return '\r';
}

