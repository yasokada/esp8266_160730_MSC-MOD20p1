#include <Wire.h>

/*
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

  Wire.begin();
  Wire.setClock(100000L); // 100kHz

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
    code = readData(DEVICE_ADDRESS);
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

char readData(char adr) {
  Wire.requestFrom(adr, /* length= */1);  
  char code = Wire.read();
  delayMicroseconds(30);

  return code;
}

bool readReply(int maxlen, char *dstPtr){
  char code;

  if (dstPtr == NULL) {
    return false;
  }
    
  for(int loop = 0; loop < maxlen; loop++) {
    code = readData(DEVICE_ADDRESS);
    if (isData(code)) {
      *dstPtr = code;
      dstPtr++;
    } else {
      return false;
    }
  }
  return true;
}

void sendData(int size, char *srcPtr) {
  for(int idx = 0; idx < size; idx++) {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write(srcPtr[idx]);
    Wire.endTransmission();
    delayMicroseconds(30);
  }
}

void checkWithAck()
{
  char sndstr[] = { 0x0A };
  int rcvlen = sizeof("!00");
  char rcvstr[5] = { 0 }; // longer than "!00" + 1

  sendData(/*size=*/1, sndstr);
  bool rcvd = readReply(rcvlen, rcvstr);
  if (rcvd) {
    Serial.print(rcvstr);
  }
}

