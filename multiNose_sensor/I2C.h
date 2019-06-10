/*
 * I2C.h
 *
 * Created: 12.11.2018 13.10.23
 *  Author: Andreas
 */ 


#ifndef I2C_H_
#define I2C_H_

// These functions are based on the tutorial by Elliot Williams in the book Make: AVR Programming.
void initI2C(void);
void i2cStart(void);
void i2cStop(void);
uint8_t i2cReadAck(void);
uint8_t i2cReadNoAck(void);
void i2cSend(uint8_t data);




#endif /* I2C_H_ */