
#include <avr/io.h>
#include "I2C.h"

// These functions are based on the tutorial by Elliot Williams in the book Make: AVR Programming.

void initI2C(void){
	TWBR = 32; 				// Setting bit rate to 100kHz, see page 267 of ATmega328P datasheet
	TWCR |= (1<<TWEN); 		// TWEN bit enables I2C operation and interface. Sets SCL and SDA pins. See page 297 of ATmega328P datasheet.
	
	/*	TWBR = TWI Bit Rate Register
		TWCR = TWI Control Register
		TWINT = TWI Interrupt Flag
		TWEA = TWI Enable Acknowledge Bit
		TWSTA = TWI Start Condition Bit
		TWSTO = TWI Stop Condition Bit
		TWWC = TWI Write Collision Flag
		TWEN = TWI Enable Bit
		Bit 1 = Reserved Bit
		TWIE = TWI Interrupt Enable
*/
}


// --- Start I2C communication. ---
void i2cStart(void){
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTA);
	loop_until_bit_is_set(TWCR,TWINT);
}

// --- Send data via I2C. ---
void i2cSend(uint8_t data){
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);               
	loop_until_bit_is_set(TWCR,TWINT);
}

// --- Read data with an acknowledgement. ---
uint8_t i2cReadAck(void){
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	loop_until_bit_is_set(TWCR,TWINT);
	return (TWDR);
}

// --- Read data without an acknowledgement. ---
uint8_t i2cReadNoAck(void){
	TWCR = (1<<TWINT) | (1<<TWEN);
	loop_until_bit_is_set(TWCR,TWINT);
	return (TWDR);
}

// --- Stop I2C communication. ---
void i2cStop(void){
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

