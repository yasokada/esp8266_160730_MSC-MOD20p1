
/*
 *   - add MSCMOD_CloseFile()
 *   - add MSCMOD_OpenFile()
 *   - add XXX_MSCMOD_testWriteAndRead()
 * v0.13 
 *   - MSDMOD_CheckFreeSpace() checks ACK
 *   - MSCMOD_CheckVersion() checks ACK
 * v0.12 add character tools
 *   - add CHR_getStringIndexOf()
 *   - add CHR_getTerminatorCount()
 * v0.11
 *   - add [kCode_terminate]
 * v0.10 2016 Oct. 01
 *   - add MSDMOD_CheckFreeSpace()
 * v0.9 2016 Oct. 01
 *   - fix typo > [eplased_msec] to [elapsed_msec]
 * v0.8 2016 Oct. 01
 *   - MSCMOD_InitSD() returns error when ack is not received
 *   - add isAck()
 * v0.7 2016 Oct. 01 > add timeout for read i2 char
 *   - MSCMOD_InitSD() uses readReply_delayAndTimeout() instead of readReply()
 *   - MSCMOD_CheckVersion() uses readReply_delayAndTimeout() instead of readReply()
 *   - readReply_delayAndTimeout() treats timeout
 *   - add readReply_delayAndTimeout()
 * v0.6 2016 Aug. 09
 *   - MSCMOD_InitSD() takes [retry:int] arg
 * v0.5 2016 Aug. 05
 *   - add receiveDummy()
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

static const char kCode_terminate = 0x0a;

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

// TOOD: 0m > treat [half data token] and [no data token]
// TODO: 0m > add length for dstPtr to avoid overflow
//
bool readReply_delayAndTimeout(int delay_msec, int timeout_msec, char *dstPtr) {
  char code;
  bool rcvd = false;
  int elapsed_msec = 0; // maximum: 32767

  if (dstPtr == NULL) {
    return false;
  }

  while(1) { // TODO: 0m > limit with maxloop
    if (elapsed_msec > timeout_msec) {
      return false;
    }
    code = i2c_readCode(DEVICE_ADDRESS);
    if (isData(code)) {
      *dstPtr = code;
      dstPtr++;
      rcvd = true;
      elapsed_msec += 1; // TODO: 0m > correct time measurement
    } else {
      if (rcvd) {
        return true;
      }
      i2c_delay(delay_msec);
      elapsed_msec += delay_msec;
    }
  }
  return rcvd;
}

// TODO: 0m > remove or rewrite using readReply_delayAndTimeout()
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

bool isAck(char *srcPtr) {
  return (strncmp(srcPtr, "!00", 3) == 0);  
}

void receiveDummy(int rcvlen)
{
  char rcvstr[20] = { 0 };

  for(int idx=0; idx<rcvlen; idx++) {
    delay(1); // msec
    (void)readReply(/*rcvlen=*/1, rcvstr);
  }
}

//--------------------------------------------------------------
// character tools
int CHR_getTerminatorCount(char *srcPtr) 
{
  if (srcPtr == NULL) {
    return 0;
  }

  int cnt = 0;
  while(*srcPtr != 0x00) {
    if (*srcPtr == kCode_terminate) {
      cnt++;
    }
    srcPtr++;
  }

  return cnt;
}

bool CHR_getStringIndexOf(char *srcPtr, int idx, int maxlen, char *dstPtr)
{
  if (srcPtr == NULL || dstPtr == NULL) {
    return false;
  }

  int pos = 0;
  int cntTrmt = 0; // count of terminator

  while(*srcPtr != 0x00) {
    if (cntTrmt == idx) {
      *dstPtr = *srcPtr;
      dstPtr++;
      pos++;
      if (pos >= maxlen) {
        break;
      }
    }
    if (*srcPtr == kCode_terminate) {
      cntTrmt++;
    }
    srcPtr++;
  }

  *dstPtr = 0x00;
  return (pos > 0);
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
  char sndstr[] = { kCode_terminate };
  i2c_sendData(/*size=*/1, sndstr);
  return receiveAck(dstPtr);
}

bool MSCMOD_InitSD(char *dstPtr, int retry)
{
  char sndstr[] = { 'I', 0x20, 'M', ':', kCode_terminate, 0x00 }; // should end with 0x00 for strlen()
  int len = strlen(sndstr);

  i2c_sendData(/*size=*/len, sndstr);

  bool rcvd = readReply_delayAndTimeout(/* delay_msec=*/10, /* timeout_msec=*/5000, dstPtr);
  if (rcvd) {
    return isAck(dstPtr);
  } else {
    return false;
  }
}

bool MSCMOD_CheckVersion(char *dstPtr)
{
  // - Example of [return characters]
  // v2.0.0<0x0A>
  // !00<0x0A>

  char sndstr[] = { 'V', kCode_terminate, 0x00 }; // should end with 0x00 for strlen()
  int len = strlen(sndstr);

  // 1. i2c read
  i2c_sendData(/*size=*/len, sndstr);
  bool rcvd = readReply_delayAndTimeout(/* delay_msec=*/10, /* timeout_msec=*/1000, dstPtr);

  char wrk[20+1] = { 0 };
  bool res;
  bool ack = false;
  // 2a. check ACK
  res = CHR_getStringIndexOf(dstPtr,/*idx=*/1, /*maxlen=*/20, wrk);
  ack = isAck(wrk);
  if (res == false || ack == false) {
    return false;
  }
  // 2b. read version info
  res = CHR_getStringIndexOf(dstPtr,/*idx=*/0, /*maxlen=*/20, wrk);
  if (res == false) {
    return false;
  }
  strcpy(dstPtr, wrk);

  return true;
}

bool MSDMOD_CheckFreeSpace(char *dstPtr)
{
  // - Example of [return characters]
  // !00<0x0A><0x00>
  // $000000006E170000<0x0A>
  // !00<0x0A><0x00>

  char sndstr[] = { 'K', 0x20, 'M', ':', kCode_terminate, 0x00 }; // should end with 0x00 for strlen()
  char rcvstr[10];
  int len = strlen(sndstr);

  i2c_sendData(/*size=*/len, sndstr);
  bool rcvd;

  // 1. receive ACK
  rcvd = readReply_delayAndTimeout(/* delay_msec=*/10, /* timeout_msec=*/1000, rcvstr);
  if (rcvd == false) {
    return false;  
  }
  if (isAck(rcvstr) == false) {
    return false;
  }

  // 2. receive free space bytes
  rcvd = readReply_delayAndTimeout(/* delay_msec=*/10, /* timeout_msec=*/1000, dstPtr);
  if (rcvd == false) {
    return false;  
  }

  char wrk[20+1] = { 0 };
  bool res;
  bool ack = false;
  // 2a. check ACK
  res = CHR_getStringIndexOf(dstPtr,/*idx=*/1, /*maxlen=*/20, wrk);
  ack = isAck(wrk);
  if (res == false || ack == false) {
    return false;
  }
  // 2b. read free space info
  res = CHR_getStringIndexOf(dstPtr,/*idx=*/0, /*maxlen=*/20, wrk);
  if (res == false) {
    return false;
  }
  strcpy(dstPtr, wrk);
  
  return true;
}

bool XXX_MSCMOD_testWriteAndRead(char *dstPtr)
{
  /*
  With ESP8266, following process will cause watchdog reset
  because it takes more than 5 seconds to receceive the reply.
  So, the following process is not implemented correctly.
  */

  // - Example of [return characters]
  // !00<0x0A><0x00>
  // $000000006E170000<0x0A>
  // !00<0x0A><0x00>

  // 400: 1kB
  char sndstr[] = { 'E', 0x20, 'M', ':', '>', '4', '0', '0', kCode_terminate, 0x00 }; // `should end with 0x00 for strlen()
  char rcvstr[30];
  int len = strlen(sndstr);

  i2c_sendData(/*size=*/len, sndstr);
  bool rcvd;

  // 2. receive process time
  rcvd = readReply_delayAndTimeout(/* delay_msec=*/10, /* timeout_msec=*/1000, dstPtr);
  if (rcvd == false) {
    return false;  
  }

  Serial.println(dstPtr);
}

bool MSCMOD_OpenFile(char *fileName, char *dstPtr)
{
  // - Example of [return characters]
  // !00<0x0A><0x00>

//  char sndstr[] = { 'O', 0x20, '1', 'R', '>', 'M', ':', '0', kCode_terminate, 0x00 }; // `should end with 0x00 for strlen()

  char sndstr[30];

  strcpy(sndstr, "O 1W>M:\\");
  strcat(sndstr, fileName);
  int len = strlen(sndstr);
  sndstr[len++] = kCode_terminate;
  sndstr[len++] = 0x00;

  i2c_sendData(/*size=*/len, sndstr);

  bool rcvd;

  // 2. receive process time
  rcvd = readReply_delayAndTimeout(/* delay_msec=*/10, /* timeout_msec=*/1000, dstPtr);
  if (rcvd == false) {
    return false;  
  }

  return true;
} 

bool MSCMOD_CloseFile(char *dstPtr)
{
  // - Example of [return characters]
  // !00<0x0A><0x00>

  char sndstr[] = { 'C', 0x20, '1', kCode_terminate, 0x00 }; // should end with 0x00 for strlen()
  int len = strlen(sndstr);

  i2c_sendData(/*size=*/len, sndstr);

  bool rcvd = readReply_delayAndTimeout(/* delay_msec=*/10, /* timeout_msec=*/5000, dstPtr);
  if (rcvd) {
    return isAck(dstPtr);
  } else {
    return false;
  }
}
