
#include <avr/io.h>
#include "GPS.h"
#include "USART.h"

void getGPSData(uint8_t* year, uint8_t* month, uint8_t* date, uint8_t* hour, uint8_t* minute, uint8_t* second, uint8_t* latitude, uint8_t* longitude){
	uint8_t GPS_data[60] = {0};
	uint8_t numbersOfValidDatas = 0;
	uint8_t commas[15] = {0};
	uint8_t test = 0;
	uint32_t latAndLongDecimalsLong = 0;
	uint16_t latAndLongDecimalsShort = 0;
	
	while(numbersOfValidDatas < 6)
	{
		// Store the RMC data category
		test = USART_Receive_byte();
		if(test == 0b01001101){						// If we get an 'M'...
			test = USART_Receive_byte();
			if(test == 0b01000011){					// ...followed by a 'C'.
				for(int i=0; i<60; i++){
					GPS_data[i] = USART_Receive_byte();
				}
				
			}
		}
		
		// The 12th character is 'A' if the data is valid (otherwise it's a 'V').
		if(GPS_data[12] == 0b01000001){
			numbersOfValidDatas += 1;
			for(int i=0; i<60; i++){
				transmitByte(GPS_data[i]);
			}
		}
		
	}
	
	
	
	// If we have gotten valid data 6 times, we can start working on it
	printString("\nWe now have our data\n");
	
	// Some number of digits can vary, but the numbers of commas are constant.
	int j = 0;
	for(int i=0; i<60; i++){
		if(GPS_data[i] == 0b00101100){
			commas[j] = i;
			j++;
		}
	}
	
	// Convert the numbers from ASCII to binary. The letters stay the same.
	for(int i=0; i<60; i++){
		GPS_data[i] = convertFromASCIIToBinary(GPS_data[i]);
	}
	
	// Store the GPS data to their respective variables.
	*hour = GPS_data[1]*10 + GPS_data[2]; // Add 2 to compensate for UTC. MUST CHANGE LATER!
	*minute = GPS_data[3]*10 + GPS_data[4];
	*second = GPS_data[5]*10 + GPS_data[6];
	*date = GPS_data[commas[8]+1]*10 + GPS_data[commas[8]+2];
	*month = GPS_data[commas[8]+3]*10 + GPS_data[commas[8]+4];
	*year = GPS_data[commas[8]+5]*10 + GPS_data[commas[8]+6];
	
	// latitude[0] and longitude[0] are uint8_t, and are the integers of the coordinates.
	// [1] and [2] are uint16_t split into two uint8_t, and are the the decimals of the coordinates.
	latitude[0] = GPS_data[14]*10 + GPS_data[15];
	latAndLongDecimalsLong = GPS_data[16]*10E5 + GPS_data[17]*10E4 + GPS_data[19]*10E3 + GPS_data[20]*10E2 + GPS_data[21]*10 + GPS_data[22];
	latAndLongDecimalsShort = latAndLongDecimalsLong/60;
	latitude[1] = ((latAndLongDecimalsShort >> 8) & 0xff);
	latitude[2] = (latAndLongDecimalsShort & 0xff);
	// MISTAKE HERE: 10E3 != 10^3. 10E3 = 10*10^3. MUST CORRECT THIS!
	longitude[0] = GPS_data[26]*100 + GPS_data[27]*10 + GPS_data[28];
	latAndLongDecimalsLong = GPS_data[29]*10E5 + GPS_data[30]*10E4 + GPS_data[32]*10E3 + GPS_data[33]*10E2 + GPS_data[34]*10 + GPS_data[35];
	latAndLongDecimalsShort = latAndLongDecimalsLong/60;
	longitude[1] = ((latAndLongDecimalsShort >> 8) & 0xff);
	longitude[2] = (latAndLongDecimalsShort & 0xff);
	
	/*
	printString("\nSec: ");
	printByte(*second);
	printString("\nMinute: ");
	printByte(*minute);
	printString("\nHour: ");
	printByte(*hour);
	printString("\nDay: ");
	printByte(*date);
	printString("\nMonth: ");
	printByte(*month);
	printString("\nYear: ");
	printByte(*year);
	printString("\nCommas: ");
	printByte(commas[8]);
	
	printString("\nLatitude: ");
	printByte(latitude[0]);
	printByte(latitude[1]);
	printByte(latitude[2]);
	
	printString("\nLongitude: ");
	printByte(longitude[0]);
	printByte(longitude[1]);
	printByte(longitude[2]);
	
	
	printString("\nlatAndLongDecimalsShort: ");
	printWord(latAndLongDecimalsShort);
	*/
	
}




uint8_t convertFromASCIIToBinary(uint8_t input){
	switch(input){
		case 0b00110000:
		return 0b00000000;
		case 0b00110001:
		return 0b00000001;
		case 0b00110010:
		return 0b00000010;
		case 0b00110011:
		return 0b00000011;
		case 0b00110100:
		return 0b00000100;
		case 0b00110101:
		return 0b00000101;
		case 0b00110110:
		return 0b00000110;
		case 0b00110111:
		return 0b00000111;
		case 0b00111000:
		return 0b00001000;
		case 0b00111001:
		return 0b00001001;
		default:
		return input;
	}
}