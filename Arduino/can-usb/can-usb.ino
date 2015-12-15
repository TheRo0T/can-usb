#include "queue.h"
#include "can.h"

#define CMD_BUFFER_LEN 30  // Lenght command buffer
#define ERR         7      // Error (ASCII BEL)

#define LED_PIN 13

struct Queue uartQueue;    // Creating a queue for UART messages
struct CanMsg canTxMsg;

uint8_t cmdBuf[CMD_BUFFER_LEN];  // command buffer
uint8_t bufIdx = 0;

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

uint8_t execCmd(uint8_t * cmdBuf) {
  
  uint8_t dataCnt;                              // counter for CAN-DATA
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
    case 'V':
      Serial.print("V0101");
      return '\r';
    
    case 't':
      if ((cmdLen < 5) || (cmdLen > 21))
                return ERR;	// check valid cmd length
    
      // store ID
      canTxMsg.id = ascii2byte(++cmdBufPntr);
      canTxMsg.id <<= 4;
      canTxMsg.id += ascii2byte(++cmdBufPntr);
      canTxMsg.id <<= 4;
      canTxMsg.id += ascii2byte(++cmdBufPntr);
      
      // store data length
      canTxMsg.len = ascii2byte(++cmdBufPntr);
      // check for valid length
      if (canTxMsg.len > 8)
        return ERR;
      
      // store data
      else {		
        for (dataCnt = 0; dataCnt < canTxMsg.len; dataCnt++) {
          canTxMsg.dataByte[dataCnt] = ascii2byte(++cmdBufPntr);
          canTxMsg.dataByte[dataCnt] <<= 4;
          canTxMsg.dataByte[dataCnt] += ascii2byte(++cmdBufPntr);
        }
      }
      
    //  return transmit_CAN ();
    
    //  Serial.print("t03680102030405060708");
    //  return '\r';
    
    default:
      return ERR;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT); 
}

void loop() {
  if (isQueueReady(&uartQueue)) {
    uint8_t rxChar = readFromQueue(&uartQueue);
    if (rxChar == '\r') {    // End command
      cmdBuf[bufIdx] = '\0'; // End string
      Serial.write(execCmd(cmdBuf));
      bufIdx = 0; 
    }
    else if (rxChar != 0) {
      cmdBuf[bufIdx++] = rxChar;
    } 
  }
}

void serialEvent() {
  writeToQueue(&uartQueue, Serial.read());
}  
