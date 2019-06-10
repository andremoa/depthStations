/*
 * USART.h
 *
 * Created: 12.11.2018 12.14.16
 *  Author: Andreas
 */ 


#ifndef USART_H_
#define USART_H_

// Some of these functions are based on the tutorial by Elliot Williams in the book Make: AVR Programming.

void initUSART(unsigned int ubrr);
void transmitByte(uint8_t data);
uint8_t USART_Receive_byte(void);
void printString(const char myString[]);
void printWord(uint16_t word);
void printByte(uint8_t byte);
void printBinaryByte(uint8_t byte);


#endif /* USART_H_ */