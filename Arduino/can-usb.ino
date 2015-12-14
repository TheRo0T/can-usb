#include <avr/interrupt.h> //библиотека прерываний
#include <string.h>

#define BUF_SIZE_RX 256 
#define BUF_SIZE_TX 256 
#define USART_DataReady  (rxIn != rxOut)    // Индексы буферов не равны, значит в буфере есть данные

#define CMD_BUFFER_LEN 30  // Длина буфера команды
#define ERR         7      // Ошибка (ASCII BEL)

//Буфер приема и его индексы:
uint8_t bufRX [BUF_SIZE_RX];
volatile uint8_t rxIn, rxOut;

//Буфер передачи и его индексы:
uint8_t bufTX [BUF_SIZE_TX];
volatile uint8_t txIn, txOut;

uint8_t rxChar;
uint8_t cmdBuf[CMD_BUFFER_LEN];  // Буфер команды
uint8_t bufIdx = 0;
 

void initUSART (void) {
   //Режим двойной скорости включен:
   UCSR0A |= (1 << U2X0);
   //Разрешены прерывание приема, работа приемника и передатчика:
   UCSR0B = (1 << RXCIE0)|(1 << RXEN0)|(1 << TXEN0);
   //Настройка размера фрейма данных 8 бит, без четности, 1 стоп-бит:
   UCSR0C = (1 << UCSZ01)|(1 << UCSZ00);
   //Настройка скорости 115200 bps:
   UBRR0H = 0;
   UBRR0L = 16;
}


uint8_t uartRead(void) {
  uint8_t readValue = 0;
    
  UCSR0B &= ~(1 << RXCIE0);      // Запрещаем прерывание приёмника
  readValue = bufRX[rxOut++];    // Считываем байт из буфера
  UCSR0B |= (1 << RXCIE0);       // Разрешаем прерывание приёмника
  
  return readValue; 
}


void uartWrite(uint8_t txData) {
  if((UCSR0B & (1 << TXCIE0)) == 0) {   // Отправляем сразу
    UDR0 = txData;
  }
  else {                        
    UCSR0B &= ~(1 << TXCIE0);   // Запрещаем прерывание передатчика
    bufTX[txIn++] = txData;     // Сохраняем в буфер
  }
  UCSR0B |= (1 << TXCIE0);    // Разрешаем прерывание передатчика
}

ISR(USART_TX_vect) {
   if(txIn != txOut) {
      UDR0 = bufTX[txOut++];
   } else {
      UCSR0B &= ~(1 << TXCIE0);   // Запрещаем прерывание передатчика
   }
}


ISR(USART_RX_vect) {
   bufRX[rxIn++] = UDR0;
}

uint8_t execCmd(uint8_t * cmdBuf) {
  
  switch (cmdBuf[0]) {
    case 'V':
      uartWrite('V');
      uartWrite('0');
      uartWrite('1');
      uartWrite('0');
      uartWrite('1');
      return '\r';
    
    case 't':
      uartWrite('t');
      uartWrite('0');
      uartWrite('3');
      uartWrite('6');
      uartWrite('5');
      uartWrite('0');
      uartWrite('1');
      uartWrite('0');
      uartWrite('2');
      uartWrite('0');
      uartWrite('3');
      uartWrite('0');
      uartWrite('4');
      uartWrite('0');
      uartWrite('5');
      return '\r';
      
    default:
      return ERR;
    
  }
    
}

uint8_t idxDiff (uint8_t idxIn, uint8_t idxOut)
{
    if (idxIn >= idxOut)
        return (idxIn - idxOut);
    else
        return ((BUF_SIZE_RX - idxOut) + idxIn);
}

void setup() {
  pinMode(13, OUTPUT); 
  initUSART();
  sei();
  
}

void loop() {
  if (USART_DataReady) {
    rxChar = uartRead();
//    uartWrite(rxChar);
    
    if (rxChar == '\r') {        // Символ конца команды
      uartWrite(execCmd(cmdBuf));
      // flush command buffer
      memset(cmdBuf, 0x00, CMD_BUFFER_LEN);
      
      bufIdx = 0;                // Индекс буфера      
    }
    
    else if (rxChar != 0) {      // Сохраняем очередной символ в буфере
      cmdBuf[bufIdx] = rxChar;
      bufIdx++;   
    }
    
    if (idxDiff(txIn, txOut) > 200){
      digitalWrite(13,HIGH);
    }
  }
}
