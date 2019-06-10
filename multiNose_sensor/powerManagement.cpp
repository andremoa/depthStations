#define F_CPU 8000000L

#include <avr/io.h>
#include <util/delay.h>
#include "powerManagement.h"

void minimizePowerWhileAwake(void){
	
	// Set all pins to input.
	DDRB = 0x00;
	DDRC = 0b00001011; //[7](nothing) [6]reset [5]SCL [4]SDA [3]clockPower [2] [1] [0]GPSMOSFET
	DDRD = 0b11100000; //[7]LoRaMOSFET [6]DepthMultiplexer [5]DepthMOSFET
	
	// Set pullup resistor on all pins.
	PORTB = 0xff;
	PORTC = 0b11110100; //[7](nothing) [6]reset [5]SCL [4]SDA [3]clockPower [2] [1]LoRaMOSFET [0]GPSMOSFET
	PORTD = 0b00011111; //[7]LoRaMOSFET [6]DepthMultiplexer [5]DepthMOSFET
	
	// Timers/counters are disabled by default on reset (they are off unless activated).
	
	// Disable ADC
	ADCSRA &= ~(1<<7);
	
	// Shut down power to all internal modules (interrupt still works).
	// [7]TWI [6]TC2 [5]TC0 [4](nothing) [3]TC1 [2]SPI [1]USART [0]ADC
	PRR = 0b01111001;
}

void goToSleep(void){
	
	/*
	PRR = 0xff;
	DDRC = 0b00111001;
	PORTC = 0b11000110;
	*/
	
	// Disable WDT!
	
	// Disable ADC
	ADCSRA &= ~(1<<7);
	
	// Go to deep sleep
	SMCR |= (1<<2); // Power down mode
	SMCR |= 1; // Enable sleep
	
	// BOD disable
	MCUCR |= (3<<5); // Set both BODS and BODSE at the same time
	MCUCR = (MCUCR & ~(1<<5)) | (1<<6); // Then set the BODS bit and clear the BODSE bit at the same time
	__asm__ __volatile__("sleep"); // In line assembler sleep execute instruction
}


void powerClockOn(void){
	DDRC |= 0b00001000;
	PORTC |= 0b00001000;
	_delay_ms(3); // The clock needs a little time to start (not much, but more than one clock cycle).
}

void powerClockOff(void){
	PORTC &= 0b11110111;
}

void powerGpsMosfetOn(void){
	DDRC |= 0b00000001;
	PORTC |= 0b00000001;
}

void powerGpsMosfetOff(void){
	PORTC &= 0b11111110;
}

void powerLoRaMosfetOn(void){
	DDRD |= 0b10000000;
	PORTD |= 0b10000000;
	_delay_ms(500); // LoRa needs a little time to start (not much, but more than one clock cycle).
}

void powerLoRaMosfetOff(void){
	PORTD &= 0b01111111;
}

void depthMultiplexerOn(void){
	DDRD |= 0b01000000;
	PORTD |= 0b01000000;
}

void depthMultiplexerOff(void){
	PORTD &= 0b10111111;
}

void depthMosfetOn(void){
	DDRD |= 0b00100000;
	PORTD |= 0b00100000;
}

void depthMosfetOff(void){
	PORTD &= 0b11011111;
}