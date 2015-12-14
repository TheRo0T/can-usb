#define BUF_SIZE_RX 256 

#define USART_DataReady  (rxIn != rxOut)

#define CMD_BUFFER_LEN 30  // Длина буфера команды
#define ERR         7      // Ошибка (ASCII BEL)

#define LED_PIN 13

//Буфер приема и его индексы:
uint8_t bufRx [BUF_SIZE_RX];
volatile uint8_t rxIn, rxOut;

uint8_t cmdBuf[CMD_BUFFER_LEN];  // Буфер команд
uint8_t bufIdx = 0;

uint8_t readBuf() {
  uint8_t tmpChar = bufRx[rxOut++];
  return tmpChar;
}

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
  
  uint8_t rxChar;
  
  if (USART_DataReady) {
    rxChar = readBuf();
//    Serial.write(rxChar);
    if (rxChar == '\r') {    // end command
      cmdBuf[bufIdx] = '\0'; // end string
      Serial.write(execCmd(cmdBuf));
      bufIdx = 0; 
    }
    else if (rxChar != 0) {
      cmdBuf[bufIdx++] = rxChar;
    } 
  }
}

void serialEvent() {
  bufRx[rxIn++] = Serial.read();
}  
