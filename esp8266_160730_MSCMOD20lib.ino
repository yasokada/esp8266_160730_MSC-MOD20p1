
/*
 * v0.4 2016 Aug. 01
 *   - modify readReply() to accept receiving with shorter length
 *   - add MSCMOD_CheckVersion()
 * v0.3 2016 Jul. 30
 *   - add MSCMOD_InitSD()
 * v0.2 2016 Jul. 30
 *   - add receiveAck()
 * v0.1 2016 Jul. 30
 *   - add readReply()
 *   - add isData()
 *   - add readBootBanner()
 */

#define DEVICE_ADDRESS (0xA4 >> 1)

//--------------------------------------------------------------
// file scope static 

bool isData(char code) {
  return (code != 0x00 && code != 0xFF);
}

bool readReply(int maxlen, char *dstPtr){
  char code;
  bool rcvd = false;

  if (dstPtr == NULL) {
    return false;
  }
    
  for(int loop = 0; loop < maxlen; loop++) {
    code = i2c_readCode(DEVICE_ADDRESS);
    if (isData(code)) {
      *dstPtr = code;
      dstPtr++;
      rcvd = true;
    } else {
      return rcvd;
    }
  }
  return true;
}

bool receiveAck(char *dstPtr)
{
  int rcvlen = sizeof("!00");
  char rcvstr[5] = { 0 }; // longer than "!00" + 1

  bool rcvd = readReply(rcvlen, rcvstr);
  if (rcvd && dstPtr != NULL) {
    strncpy(dstPtr, rcvstr, rcvlen);
  } 

  return (strncmp(rcvstr, "!00", 3) == 0);
}

//--------------------------------------------------------------
// public function

void MSCMOD_ReadBootBanner(int rcvmaxlen, char *dstPtr)
{
  if (dstPtr == NULL) {
    return; // error
  }

  char code;
  for(int loop=0; loop < 100; loop++) {
    code = i2c_readCode(DEVICE_ADDRESS);
    if (isData(code)) {
      *dstPtr = code;
      dstPtr++;
    } else {
      break;
    }
  }
}

bool MSCMOD_CheckWithAck(char *dstPtr)
{
  char sndstr[] = { 0x0A };
  i2c_sendData(/*size=*/1, sndstr);

  return receiveAck(dstPtr);
}

bool MSCMOD_InitSD(char *dstPtr)
{
  char sndstr[] = { 'I', 0x20, 'M', ':', 0x0A, 0x00 }; // should end with 0x00 for strlen()
  int len = strlen(sndstr);

  i2c_sendData(/*size=*/len, sndstr);
  return receiveAck(dstPtr);
}

bool MSCMOD_CheckVersion(char *dstPtr)
{
  char sndstr[] = { 'V', 0x0A, 0x00 }; // should end with 0x00 for strlen()
  int len = strlen(sndstr);

  i2c_sendData(/*size=*/len, sndstr);
  bool rcvd = readReply(/* maxlen=*/12, dstPtr);

//  Serial.println(dstPtr);
  
  return true;
}

