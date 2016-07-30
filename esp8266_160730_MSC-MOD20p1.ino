#include <Wire.h>
#include "esp8266_160730_i2cWrapper.h"

/*
 * v0.5 2016 Jul. 30
 *   - separate i2c setup/read/write to external file
 *   - rename readData() to readCode()
 *   - readData() takes adr param
 * v0.4 2016 Jul. 30
 *   - rename helloWorld() to checkWithAck()
 *   - add sendData()
 *   - rename readAck() to readReply()
 *   - readAck() returns bool type
 * v0.3 2016 Jul. 30
 *   - add readAck()
 *   - add isData()
 *   - readData() returns char type
 *   - add readBootBanner()
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

  i2c_setup();
  readBootBanner();
}

void loop() {
  checkWithAck();
  delay(3000); // msec
}

void readBootBanner()
{
  char code;
  for(int loop=0; loop<100; loop++) {
    code = i2c_readCode(DEVICE_ADDRESS);
    if (isData(code)) {
      Serial.print(code);
    } else {
      break;
    }
  }
}

bool isData(char code) {
  return (code != 0x00 && code != 0xFF);
}

bool readReply(int maxlen, char *dstPtr){
  char code;

  if (dstPtr == NULL) {
    return false;
  }
    
  for(int loop = 0; loop < maxlen; loop++) {
    code = i2c_readCode(DEVICE_ADDRESS);
    if (isData(code)) {
      *dstPtr = code;
      dstPtr++;
    } else {
      return false;
    }
  }
  return true;
}

void checkWithAck()
{
  char sndstr[] = { 0x0A };
  int rcvlen = sizeof("!00");
  char rcvstr[5] = { 0 }; // longer than "!00" + 1

  i2c_sendData(/*size=*/1, sndstr);
  bool rcvd = readReply(rcvlen, rcvstr);
  if (rcvd) {
    Serial.print(rcvstr);
  }
}

