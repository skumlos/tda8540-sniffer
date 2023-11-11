/* TDA8540 Channel Sniffer
 * (2023) Martin Hejnfelt (martin@hejnfelt.com)
 *
 * For use with Barco/BarcoNet/Scientific Atlanta ADVM monitors.
 *
 * This sniffs which channel is selected, and sets a port accordingly.
 *
 * For now its only use case is on the Barco / BarcoNet /Scientific Atlanta ADVM
 * to know which composite input is selected, and use that information
 * on another board to provide blanking to the TDA9321H jungle IC.
 *
 * Connect the main I2C line of the monitors PCB to the 
 * hardware I2C (TWI) pins through a non-inverting buffer, and a series 100 Ohm
 * resistor. Connect pin 13 to the input pin of the TDA9321H unit. When channel 2
 * is selected the output is HIGH, when 1 is selected, it is LOW.
 *
 * On the Barco ADVM14, SCL is R139 and SDA is R138 of the TDA8540 but
 * you an also just connect to the ports of the other Arduino where I assume
 * the TDA9321H software is running.
 *  
 * Monitors tested:
 * BarcoNet ADVM14
 *
 * Expect your monitor to catch fire!
 */

// Version 1.0

#define LOOP_DELTA_MS (10)
#define GPIO_CHANNEL (13) // LED

#include <Wire.h>

#define TDA8540_ADDR  (72)
#define REG_8540_SW1 (0x00)

uint8_t input8540 = 0;

enum WriteState {
  WR_REG,
  WR_DATA
};

uint8_t w8540[3] = { 0, 0, 0 };
WriteState writeState8540 = WR_REG;
int currentReg8540 = 0x0;

void writeRequest8540(int byteCount) {
  for(int i=0; i<byteCount; ++i) {
    switch(writeState8540) {
      case WR_REG:
        currentReg8540 = Wire.read();
        writeState8540 = WR_DATA;
      break;
      case WR_DATA:
        w8540[currentReg8540] = Wire.read();
        switch(currentReg8540) {
          case REG_8540_SW1:
            // When CVBS input 1 / CODED A is selected,
            // 0x80 (1000 0000b) is written to SW1 register.
            // When CVBS input 2 / CODED B is selected,
            // 0xC0 (1100 0000b) is written to SW1 register.
            if(w8540[currentReg8540] & 0x40) {
              input8540 = 2;
            } else {
              input8540 = 1;
            }
          break;
        }
        currentReg8540++;
      break;
    }
  }
  writeState8540 = WR_REG;
  currentReg8540 = 0x0;
}

void readRequest8540() {
  currentReg8540 = 0x0;
}

void setup() {
  pinMode(GPIO_CHANNEL,OUTPUT);
  digitalWrite(GPIO_CHANNEL,LOW);
  Wire.begin(TDA8540_ADDR);
  Wire.onReceive(writeRequest8540);
  Wire.onRequest(readRequest8540);
}

bool blanking = false; 

void loop() {
  if(input8540 == 1 && !blanking) {
    digitalWrite(GPIO_CHANNEL,LOW);
    blanking = true;
  } else if(input8540 == 2 && blanking){
    digitalWrite(GPIO_CHANNEL,HIGH);
    blanking = false;
  }
  delay(LOOP_DELTA_MS);
}
