
/*
 * v0.1 2016 Jul. 30
 *   - add readBootBanner();
 */

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

