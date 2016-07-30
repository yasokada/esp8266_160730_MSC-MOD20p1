#include <Wire.h>
#include "esp8266_160730_i2cWrapper.h"

/*
 * v0.7 2016 Jul. 30
 *   - MSCMOD20lib: add MSCMOD_InitSD()
 *   - MSCMOD20lib: add receiveAck()
 * v0.6 2016 Jul. 30
 *   - separate MSC-MOD20 functions to external file
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

void setup() {
  Serial.begin(115200);
  Serial.println("");

  i2c_setup();
  char rcvstr[100] = { 0 };
  MSCMOD_ReadBootBanner(/*rcvmaxlen=*/100, rcvstr);
  if (strlen(rcvstr) > 0) {
    Serial.print(rcvstr);
  }
}

void loop() {  

  if (MSCMOD_InitSD(/* dstPtr=*/NULL)) {
    Serial.println("init SD: OK");
  } else {
    Serial.println("init SD: NG");    
  }
  
//  char rcvstr[5] = { 0 };
//  if (MSCMOD_CheckWithAck(rcvstr)) {
//    Serial.print(rcvstr);
//  } else {
//    Serial.println("ACK NG");
//  }
  
  delay(3000); // msec
}


