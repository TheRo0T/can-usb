#include <SPI.h>

#include <mcp_can.h>
#include <mcp_can_dfs.h>

#include "can.h"

#define CMD_BUFFER_LEN 30  // Lenght command buffer
#define ERR         7      // Error (ASCII BEL)

#define LED_PIN 13
const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin


struct CanMsg canTxMsg, canRxMsg;

uint8_t cmdBuf[CMD_BUFFER_LEN];  // command buffer
uint8_t bufIdx = 0;

uint8_t transmitCan() {
  CAN.sendMsgBuf(canTxMsg.id, 0, canTxMsg.len, canTxMsg.dataByte);    // 0 - standart message
  return '\r';
}


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
      
      return transmitCan ();
    
    //  Serial.print("t03680102030405060708");
    //  return '\r';
    
    default:
      return ERR;
  }
}

void stopAndBlink(void) {
  while(1) {
    digitalWrite(LED_PIN, HIGH);
    delay(300);
    digitalWrite(LED_PIN, LOW);
    delay(300);  
  }  
}

void setup() {
  Serial.begin(115200);
  CAN.begin(CAN_125KBPS);
//  attachInterrupt(0, MCP2515_ISR, FALLING); // start interrupt
  pinMode(LED_PIN, OUTPUT); 
  delay(100);
}

void loop() {
  
  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    
    CAN.readMsgBuf(&canRxMsg.len, canRxMsg.dataByte);
    canRxMsg.id = CAN.getCanId();
    Serial.write('t');

    // id
    Serial.write(nibble2ascii(canRxMsg.id >> 8));
    Serial.write(nibble2ascii(canRxMsg.id >> 4));
    Serial.write(nibble2ascii(canRxMsg.id));
            
    // len
    Serial.write(nibble2ascii(canRxMsg.len));
            
    // data
    for (int i=0; i < canRxMsg.len; i++) {
       Serial.write(nibble2ascii((canRxMsg.dataByte[i])>>4));
       Serial.write(nibble2ascii(canRxMsg.dataByte[i]));
    }
    
    Serial.write('\r');

    if (Serial.availableForWrite() < 3) stopAndBlink();
/*    
    // debug
    char tmp = Serial.availableForWrite();
    Serial.print("t5551");
    Serial.write(nibble2ascii(tmp>>4));
    Serial.write(nibble2ascii(tmp));
    Serial.write('\r');
*/
  }  
  
}

void serialEvent() {
  while (Serial.available()) {
    uint8_t rxChar = Serial.read();
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

/*
void MCP2515_ISR()
{

}
*/
