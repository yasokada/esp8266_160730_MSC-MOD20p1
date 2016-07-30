#include <Wire.h>

/*
 * v0.2 2016 Jul. 30
 *   - fix bug > Device Address was not 7 bit 
 * v0.1 2016 Jul. 30
 *   - add readData()
 *   - add helloWorld()
 *   - add loop()
 *   - add setup()
 */

#define DEVICE_ADDRESS (0xA4 >> 1)

void setup() {
  Serial.begin(115200);
  Serial.println("");

  Wire.begin();
  Wire.setClock(100000L); // 100kHz
}

void loop() {
  helloWorld();
  delay(3000); // msec
}

void readData() {
  Wire.requestFrom(DEVICE_ADDRESS, /* length= */1);  
  char code = Wire.read();
  delayMicroseconds(30);

  Serial.print(code);
}

void helloWorld()
{
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(0x0A);
  Wire.endTransmission(); 
  delayMicroseconds(30);

  for(int loop=0; loop<3; loop++) {
    readData();
  }
}

