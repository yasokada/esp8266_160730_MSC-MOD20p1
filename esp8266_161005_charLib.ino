
// Character library

/*
 * v0.1 2016 Oct. 05
 *  - add CHAR_ToHex() 
 */

//-------------------------------------------------------------------------

// For non-ACSII-code environment where 'A' .. 'F' might not be continuous,
// uncomment the following
//#define NON_ACSII 

bool CHAR_ToHex(int val, char *dstPtr) {
  int bit = 16;
  int nibble;
  
  if (dstPtr == NULL) {
    return false;
  }
  
  // 1. zero suppress
  while(bit > 0) {
//  printf("bit=%d %d\n", bit, (val >> bit));
    if ( ((val >> bit) & 0xF) > 0) {
      break;
    }
    bit -= 4;
  }

  // 2. conversion
  for(; bit >= 0; bit -= 4) {
    nibble = (val >> bit) & 0xF;
#ifdef NON_ACSII
    *dstPtr++ = "0123456789ABCDEF"[nibble];
#else    
    *dstPtr++ = nibble + (nibble < 10 ? '0' : ('A' - 10));
#endif    
  }
  *dstPtr = 0x0;
  return true;
}

