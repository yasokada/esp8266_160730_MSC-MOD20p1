#include <Wire.h>
#include "esp8266_160730_i2cWrapper.h"

/* 
 *   - MSCMOD20lib: v0.14 > add MSCMOD_OpenFile() and MSCMOD_CloseFile()
 *   - add Test_SDtestReadWrite()
 * v0.12 2016 OCt. 02
 *   - MSCMOD20lib: v0.13
 *   - MSCMOD20lib: v0.11, v0.12 > add character tools
 *   - add Test_SDfreeSpace()
 *   - MSCMOD20lib: v0.10 > add MSDMOD_CheckFreeSpace()
 *   - MSCMOD20lib: v0.9 > fix typo
 * v0.11 2016 Oct. 01
 *   - MSCMOD20lib: v0.8 > MSCMOD_InitSD() returns error when ack is not received
 *   - MSCMOD20lib: v0.7 > add timeout for read i2 char
 * v0.10 2016 Sep. 29
 *   - i2cwrapper: v0.2 > add i2c_delay()
 * v0.9 2016 Aug. 09
 *   - check SDInit timing with SD unmounted (3.3 sec)
 *     + Test_SDInit() takes [retry:int] arg
 * v0.8 2016 Aug. 01
 *   - add Test_SDversion()
 *   - MSCMOD20lib: add MSCMOD_CheckVersion()
 *   - add GPIO14 for Analog Discovery 2 trigger
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

static const int kPinTrigger = 14;

void setup() {
  Serial.begin(115200);
  Serial.println("");

  pinMode(kPinTrigger, OUTPUT);

  i2c_setup();
  char rcvstr[100] = { 0 };
  MSCMOD_ReadBootBanner(/*rcvmaxlen=*/100, rcvstr);
  if (strlen(rcvstr) > 0) {
    Serial.print(rcvstr);
  }  

#if 0 // comment out for check i2c read function modification
  // TODO: add function with retry to initialize SD
  Test_SDInit(/*retry=*/0);
  delay(200); // msec // x 20, 50, 100
  digitalWrite(kPinTrigger, 0);
  delay(2); // msec;
  digitalWrite(kPinTrigger, 1);
  Test_SDInit(/*retry=*/3500);  
#endif  
  
}

void Test_SDversion()
{
  char rcvstr[15] = { 0 };
  if (MSCMOD_CheckVersion(rcvstr)) {
    Serial.print(rcvstr);
  } else {
    Serial.println("Check Version: Failed");
  }
}

void Test_SDInit(int retry)
{
  char rcvstr[20] = { 0 };
  if (MSCMOD_InitSD(rcvstr, retry)) {
    Serial.println("init SD: OK");
  } else {
    Serial.println("init SD: NG");    
  }
  Serial.print("rcvd:");
  Serial.println(rcvstr);
}

void Test_SDfreeSpace()
{
  char rcvstr[30] = { 0 };
  if ( MSDMOD_CheckFreeSpace(rcvstr) ) {
    Serial.println("SD free space:");
    Serial.println(rcvstr);
  } else {
    Serial.println("SD free space: NG");    
  }
}

void Test_SDtestReadWrite()
{
  // TODO: 2z> under construction

  static bool started = false;
  char rcvstr[50] = { 0 };
  if (started == false) {
    XXX_MSCMOD_testWriteAndRead(rcvstr);
    started = true;
  } else {
    
  }
}

void Test_SDopenWriteClose()
{
  char filename[] = "TEST.TXT";
  char rcvstr[50] = { 0 };

  if (MSCMOD_OpenFile(filename, rcvstr)) {
    Serial.print(F("FileOpen "));
    Serial.println(filename);
    Serial.println(rcvstr);
  } else {
    Serial.println(F("OpenFile: Fail"));
    Serial.println(rcvstr);
  }
  //
  rcvstr[0] = 0x00;
  if (MSCMOD_CloseFile(rcvstr)) {
    Serial.print(F("FileClose: "));
    Serial.println(rcvstr);
  } else {
    Serial.println(F("CloseFile: Fail"));    
    Serial.println(rcvstr);
  }
}

void loop() {
  Test_SDversion();

  Test_SDInit(/*retry=*/0);
//  Test_SDfreeSpace();

  Test_SDopenWriteClose();
//  Test_SDtestReadWrite();

//  char rcvstr[5] = { 0 };
//  if (MSCMOD_CheckWithAck(rcvstr)) {
//    Serial.print(rcvstr);
//  } else {
//    Serial.println("ACK NG");
//  }
  
  delay(3000); // msec
}


