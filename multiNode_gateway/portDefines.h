/*
Setting a pin as output: 
	REG_PORT_DIRSET0 = PORT_PB12; // Sets port PA12 to output.
	REG_PORT_DIRSET1 = PORT_PB08; // Sets port PB08 to output.
	
	or directly in the DIR register:
	REG_PORT_DIR0 |= PORT_PB12; // Sets port PA12 to output.
	REG_PORT_DIR1 |= PORT_PB08; // Sets port PB08 to output.
	
	In the same way, DIRCLR and DIRTGL clears and toggles the pin(s).
	Setting values to 0 in DIRSET, DIRCLR and DIRTGL has no effect. Only setting bits to 1 has effect. Logical bitwise operations are therefore not needed here.
*/

/*
Setting a pin as high: 
	REG_PORT_OUTSET0 = PORT_PB12; // Sets PA12 high.
	REG_PORT_OUTSET1 = PORT_PB08; // Sets PB08 high.
	
	or directly in the DIR register:
	REG_PORT_OUT0 |= PORT_PB12; // Sets PA12 high.
	REG_PORT_OUT1 |= PORT_PB08; // Sets PB08 high.
	
	In the same way, OUTCLR and OUTTGL clears (sets to low) and toggles the pin(s).
	Setting values to 0 in OUTSET, OUTCLR and OUTTGL has no effect. Only setting bits to 1 has effect. Logical bitwise operations are therefore not needed here.
*/

/*
Setting a pin as input pullup:
	PORT->Group[0].PINCFG[2].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN; // Sets pin 2 (PINCFG[2]) in the A bank (Group[0]) to innput and enable pull-resistor (PA02).
	REG_PORT_OUTSET0 = PORT_PA02; // Set pullup.
	
	alternatively in two operations:
	PORT->Group[0].PINCFG[2].bit.INEN = 1;  
	PORT->Group[0].PINCFG[2].bit.PULLEN = 1; 
	REG_PORT_OUTSET0 = PORT_PA02; // Set pullup.
*/

/*
Checking the pin state of an input pin:
	if((PORT->Group[0].IN.reg & PORT_PA02) != 0){
		// do something
	}
	
	can also do it by reading from REG_PORT_INx:
	if((REG_PORT_IN0 & PORT_PA02) != 0){    
		//do something
	}
*/


#define PB07 (1ul << 7)
#define PB08 (1ul << 8)	// ul is for unsigned long. Same as: 0b00000000000000000000000100000000
