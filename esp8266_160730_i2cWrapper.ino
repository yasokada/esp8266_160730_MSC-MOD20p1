#include <Wire.h>
#include "esp8266_160730_i2cWrapper.h"

/*
 * v0.2 2016 Sep. 29
 *   - add i2c_delay()
 * v0.1 2016 Jul. 30
 *   - add i2c_setup()
 *   - add i2c_readCode()
 *   - add sendData()
 */

void i2c_setup() {
  Wire.begin();
  Wire.setClock(100000L); // 100kHz  
}

char i2c_readCode(char adr) {
  Wire.requestFrom(adr, /* length= */1);  
  char code = Wire.read();
  delayMicroseconds(30);

  return code;
}

void i2c_sendData(int size, char *srcPtr) {
  for(int idx = 0; idx < size; idx++) {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write(srcPtr[idx]);
    Wire.endTransmission();
    delayMicroseconds(30);
  }
}

void i2c_delay(int msec) {
  delay(msec);
}

