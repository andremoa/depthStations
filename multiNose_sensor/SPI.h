/*
 * SPI.h
 *
 * Created: 17.11.2018 15.43.33
 *  Author: Andreas
 */ 


#ifndef SPI_H_
#define SPI_H_


// SPI Defines
#define SPI_DDR DDRB
#define SPI_PORT PORTB

#define MOSI_PIN PORTB3
#define MISO_PIN PORTB4
#define SCK_PIN PORTB5
#define SS_PIN PORTB2

#define SS_SELECT SPI_PORT &= ~(1<<SS_PIN)
#define SS_DESELECT SPI_PORT |= (1<<SS_PIN)

void initSPI_master(void);
void SPI_tradeByte(uint8_t data);
void SPI_writeToRegister(uint8_t address, uint8_t data);
uint8_t SPI_readFromRegister(uint8_t address);


#endif /* SPI_H_ */