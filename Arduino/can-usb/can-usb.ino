#include <SPI.h>
#include "mcp_can.h"

#define CMD_BUFFER_LEN 30  // Lenght command buffer
#define ERR         7      // Error (ASCII BEL)
#define LED_PIN 3

struct can_frame canTxMsg, canRxMsg;
uint8_t cmdBuf[CMD_BUFFER_LEN];  // command buffer
uint8_t *cmdBufPtr = cmdBuf;      // command buffer pointer
uint8_t flagRecv = 0;            // interrupt flag

MCP_CAN mcp2515(10, MCP_CAN::MODE_LOOPBACK);

uint8_t ascii2byte (uint8_t * val) {
  uint8_t temp = *val;
  if (temp > 0x60) temp -= 0x27;                // convert chars a-f
  else if (temp > 0x40) temp -= 0x07;                // convert chars A-F
  temp -= 0x30;                                        // convert chars 0-9
  return temp & 0x0F;
}


uint8_t nibble2ascii(uint8_t nibble) {
  uint8_t tmp = nibble & 0x0f;
  return tmp < 10 ? tmp + 48 : tmp + 55;
}


uint8_t transmitCan() {
  mcp2515.sendMessage(&canTxMsg);
  return '\r';
}


uint8_t execCmd(uint8_t * cmdBuf) {
  
  uint8_t dataCnt;                              // counter for canTxMsg.dataByte
  uint8_t cmdLen = strlen ((char *)cmdBuf);	// get command length
  
  uint8_t *cmdBufPntr = &(*cmdBuf);	        // point to start of received string
  cmdBufPntr++;		                        // skip command identifier
  // check if all chars are valid hex chars
  while (*cmdBufPntr) {
    if (!isxdigit (*cmdBufPntr))
        return ERR;
    ++cmdBufPntr;
  }
  
  
  cmdBufPntr = &(*cmdBuf);	// reset pointer
  
  switch (*cmdBufPntr) {
    case 'N':
      Serial.print("N0001");
      return '\r';
      
    case 'v':
      Serial.print("v0101");
      return '\r';
      
    case 'V':
      Serial.print("V0101");
      return '\r';
    
    case 't':
          
      if ((cmdLen < 5) || (cmdLen > 21))
        return ERR;	// check valid cmd length
    
      // store ID
      canTxMsg.can_id = ascii2byte(++cmdBufPntr);
      canTxMsg.can_id <<= 4;
      canTxMsg.can_id += ascii2byte(++cmdBufPntr);
      canTxMsg.can_id <<= 4;
      canTxMsg.can_id += ascii2byte(++cmdBufPntr);

      
      // store data length
      canTxMsg.can_dlc = ascii2byte(++cmdBufPntr);
      // check for valid length
      if (canTxMsg.can_dlc > 8)
        return ERR;
      
      // store data
      else {		
        for (dataCnt = 0; dataCnt < canTxMsg.can_dlc; dataCnt++) {
          canTxMsg.data[dataCnt] = ascii2byte(++cmdBufPntr);
          canTxMsg.data[dataCnt] <<= 4;
          canTxMsg.data[dataCnt] += ascii2byte(++cmdBufPntr);
        }
      }
      
      return transmitCan ();

    
    case 'C':
    case 'O':
    case 'S':
    case 's':
    case 'L':
    case 'W':
    case 'Z':
      return '\r';
    
    default:
      return ERR;
  }
}


void setup() {
  Serial.begin(115200);
  mcp2515.begin(CAN_125KBPS);
  delay(100);
  attachInterrupt(0, MCP2515_ISR, FALLING); // start interrupt
}


void MCP2515_ISR() {
    flagRecv = 1;
}


void loop() {
  if(flagRecv) { 
    
    flagRecv = 0;                   // clear flag
    
    if (mcp2515.readMessage(&canRxMsg) == MCP_CAN::ERROR_OK) {
  
      char out[30];
      char *ptr = out;
    
      *ptr++ = 't';

      // id
      *ptr++ = nibble2ascii(canRxMsg.can_id >> 8);
      *ptr++ = nibble2ascii(canRxMsg.can_id >> 4);
      *ptr++ = nibble2ascii(canRxMsg.can_id);
            
      // len
      *ptr++ = nibble2ascii(canRxMsg.can_dlc);
            
      // data
      for (int i=0; i < canRxMsg.can_dlc; i++) {
       *ptr++ = nibble2ascii(canRxMsg.data[i] >> 4);
       *ptr++ = nibble2ascii(canRxMsg.data[i]);
      }
    
      *ptr++ = '\r';
      *ptr++ = '\0';

      Serial.print(out);
    }
  }
  
  
  // Read from serial
  if (Serial.available()) {
    uint8_t rxChar = Serial.read();
    if (rxChar == '\r') {    // End command
      *cmdBufPtr = '\0';     // End string
      uint8_t res = execCmd(cmdBuf);
      if (res == ERR) 
        digitalWrite(LED_PIN, HIGH);
      cmdBufPtr = cmdBuf;   // reset pointer
    }
    else if (rxChar != 0) {
      *cmdBufPtr++ = rxChar;
    } 
  }
}
