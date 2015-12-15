#include "queue.h"

#define CMD_BUFFER_LEN 30  // Lenght command buffer
#define ERR         7      // Error (ASCII BEL)

#define LED_PIN 13

struct queue uartQueue;    // Creating a queue for UART messages

uint8_t cmdBuf[CMD_BUFFER_LEN];  // command buffer
uint8_t bufIdx = 0;

uint8_t execCmd(uint8_t * cmdBuf) {
  
  switch (cmdBuf[0]) {
    case 'V':
      Serial.print("V0101");
      return '\r';
    
    case 't':
      Serial.print("t03680102030405060708");
      return '\r';
    
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
