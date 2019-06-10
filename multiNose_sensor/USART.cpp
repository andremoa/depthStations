
#include <avr/io.h>
#include "USART.h"

// Some of these functions are based on the tutorial by Elliot Williams in the book Make: AVR Programming.

void initUSART(unsigned int ubrr)
{
	// --- Set the baud rate ---
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	
	// --- Enable both receiver and transmitter ---
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	
	// --- Set frame format: 8data, 1stop bit ---
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

void transmitByte(uint8_t data) {
	// Wait until transmit buffer becomes empty
	loop_until_bit_is_set(UCSR0A, UDRE0);
	
	// Send the data.
	UDR0 = data;                                           
}


// Prints a string to the serial monitor.
void printString(const char myString[]) {
	uint8_t i = 0;
	while (myString[i]) {
		transmitByte(myString[i]);
		i++;
	}
}


// Prints one byte of data in dec format.
void printByte(uint8_t byte) {
	transmitByte('0' + (byte / 100));              // 10^2
	transmitByte('0' + ((byte / 10) % 10));        // 10^1
	transmitByte('0' + (byte % 10));               // 10^0
}


// Prints one byte in binary format.
void printBinaryByte(uint8_t byte) {
	uint8_t bit;
	for (bit = 7; bit < 255; bit--) {
		if (bit_is_set(byte, bit))
		transmitByte('1');
		else
		transmitByte('0');
	}
}


uint8_t USART_Receive_byte(void){
	
	/// Wait for completed data reception.
	// The RXC flag, if 1, indicates that there are unread data in the receive buffer.
	while ( !(UCSR0A & (1<<RXC0)) );   
	
	// Get and return received data from buffer.
	return UDR0;
}

void printWord(uint16_t word) {
	transmitByte('0' + (word / 10000));             // 10^4
	transmitByte('0' + ((word / 1000) % 10));       // 10^3
	transmitByte('0' + ((word / 100) % 10));        // 10^2
	transmitByte('0' + ((word / 10) % 10));         // 10^1
	transmitByte('0' + (word % 10));                // 10^0.
}