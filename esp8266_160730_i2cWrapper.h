#ifndef ESP8266_160730_I2CWRAPPER_H
#define ESP8266_160730_I2CWRAPPER_H

void i2c_setup();
char i2c_readCode(char adr);
void i2c_sendData(int size, char *srcPtr);

#endif