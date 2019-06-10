
#include <avr/io.h>
#include <SPI.h>

void initSPI_master(void){
	// Set MOSI, SCK and SS as OUTPUT
	SPI_DDR	|= (1<<MOSI_PIN) | (1<<SCK_PIN) | (1<<SS_PIN);
	
	// Set SS pin HIGH (deactivate)
	SS_DESELECT;
	
	// Set MISO as INPUT PULLUP
	SPI_DDR &= ~(1<<MISO_PIN);
	SPI_PORT |= (1<<MISO_PIN);
	
	// SPI Control Register 0
	// SPE written to 1 enables SPI. Must be set to enable any SPI operations.
	// MSTR written to 1 selects Master SPI mode.
	// [SPR1, SPR0] = [1, 0] sets SCK frequency as F_CPU/64.
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR1);
	
}

void SPI_tradeByte(uint8_t data){
	SPDR = data;
	loop_until_bit_is_set(SPSR, SPIF);
	// SPDR will now contain the byte recieved via bit-traiding.
}


void SPI_writeToRegister(uint8_t address, uint8_t data){
	SS_SELECT;
	SPI_tradeByte(address);
	SPI_tradeByte(data);
	SS_DESELECT;
}


uint8_t SPI_readFromRegister(uint8_t address){
	SS_SELECT;
	SPI_tradeByte(address);
	SPI_tradeByte(0);
	SS_DESELECT;
	return (SPDR);
}

