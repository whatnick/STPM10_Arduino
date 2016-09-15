/*
 * STPM10 interface code as outline by "TisteAndii"
 * http://arduino.stackexchange.com/questions/23684/stpm10-using-arduino-spi-communication/23742#23742
 */

#include <SPI.h>

#define SCS 4  // define your own CS to have better control
#define SYN 5  // define a SYN pin

void setup(){
  pinMode(SS, OUTPUT);  //needed to use the SPI

  pinMode(SCS, OUTPUT);  // set SCS and SYN as output
  pinMode(SYN, OUTPUT);
  digitalWrite(SCS, HIGH);  // idle state
  digitalWrite(SYN, HIGH);
  SPI.beginTransaction(SPISettings(32000000, MSBFIRST, SPI_MODE3)); // mode 3, max read clock is 32mhz
}

void write_reg(byte addr, byte val){
  val = (val << 7) | (addr << 1);  // 6-bit addresses!
  SPI.end();  // disable SPI
  pinMode(MISO, OUTPUT);
  digitalWrite(SYN, HIGH);
  delayMicroseconds(10);

  digitalWrite(SCS, LOW);  // assert SCS and SYN
  digitalWrite(SYN, LOW);
  for (int i = 0; i < 8; i++){  // shift out a byte
    delayMicroseconds(10);
    digitalWrite(SCK, LOW);  // max clock is 100kHz
    digitalWrite(MISO, (val & 0x80) ? HIGH : LOW);
    delayMicroseconds(10);
    digitalWrite(SCK, HIGH);
    val <<= 1;
  }

  digitalWrite(SYN, HIGH); // deassert SCS and SYN
  digitalWrite(SCS, HIGH);

  SPI.beginTransaction(SPISettings(32000000, MSBFIRST, SPI_MODE3)); //restart SPI
}

void read_regs(uint32_t data[], uint8_t len){  // an array to hold register data is passed
  digitalWrite(SYN, LOW);  // latching operation
  digitalWrite(SCS, LOW);
  digitalWrite(SYN, HIGH);

  uint32_t val = 0;
  for (int i = 0; i < len; i++){  // read the 8 32-bit registers into the array
    val = SPI.transfer(0);  // your 32 serial clocks
    val |= (uint32_t)SPI.transfer(0) << 8;
    val |= (uint32_t)SPI.transfer(0) << 16;
    val |= (uint32_t)SPI.transfer(0) << 24;
    data[i] = val;
  }

  digitalWrite(SCS, HIGH);  // deassert SCS
}

// Use the functions this way:

uint32_t reg_data[8];  // array to hold read data, max is 8

void loop(){
  write_reg(47, 1);  // set config bit 47
  read_regs(reg_data, 8);  // read all reg data into array

  for (int i = 0; i < 8; i++)
    Serial.println(reg_data[i], HEX);
}
