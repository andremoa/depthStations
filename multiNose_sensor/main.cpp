/*
 * TheSender.cpp
 *
 * Created: 12.11.2018 12.08.55
 * Author : Andreas
 
 4/PD2		- Clock interrupt (SQW).
 
 23/PC0		- GPS MOSFET
 
 25/PC3		- Clock power pin
 
 See schematics for full setup.
 Be sure to check whether the pins used in the code matches the ones in the schematics.
 The code or the schematics might have been updated without the other one also being updated.
 
 Make sure to import and link all files properly for the code to compile.
 
 */ 

#define F_CPU 8000000L
#define BAUD 9600
#define UBBRValue F_CPU/16/BAUD-1

#include <avr/io.h>
#include <util/delay.h>
#include "USART.h"
#include "GPS.h"
#include "I2C.h"
#include "clock.h"
#include "powerManagement.h"
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "SDI12.h"
#include "SPI.h"
#include "LoRa.h"
#include <stdint.h>
#include "Framebuffer.h"
#include <avr/pgmspace.h>
#include "bitPatterns.h"

// Address 1-99 are for nodes, 101-199 are for repeaters, 201-255 are for gateways.
#define SENDER_ADDRESS 22
#define RECEIVER_ADDRESS 110

#define USER_INTERFACE_SWITCH PINC2
#define DISPLAY_POWER_PIN PINC3
#define CLOCK_POWER_PIN PINC1

#define NEXT_ALARM_MINUTE 0
#define NEXT_ALARM_SECOND 1
uint8_t year, month, date, hour, minute, second, latitude[3], longitude[3];
uint8_t temperature;
uint8_t depthMSB, depthLSB;
uint8_t *EEPROM_address = (uint8_t *) 0;
uint8_t Sender_Address, Receiver_Address;
uint8_t packageNumber;
uint8_t correctTimeSecond;

uint16_t depth = 0;
uint16_t currentDepth = 0;
uint16_t sensorDepth = 0;
uint16_t depthCompensation = 0;
uint16_t depthCompensationSign = 0;
uint8_t fakeWakeup = 1;




int main(void) {

	_delay_ms(1); // Give a little time to clear interference from the reset.
	
	// ----- Set default pin states. -----
	minimizePowerWhileAwake();
	DDRC &= 0b11111011; PORTC |= 0b00000100; _delay_ms(10); // Set user interface switch pin to input high.
	
	// ----- If the interface switch is pulled low (i.e. activated), go through the setup. -----
	if(bit_is_clear(PINC,USER_INTERFACE_SWITCH)){ 	
		DDRC |= (1<<DISPLAY_POWER_PIN); PORTC |= (1<<DISPLAY_POWER_PIN); // Set display power pin to output high.
		DDRC |= (1<<CLOCK_POWER_PIN); PORTC |= (1<<CLOCK_POWER_PIN); // Set clock power pin to output high.
		_delay_ms(100); // The display needs some time to power up before initiating I2C.
		
		initUSART(UBBRValue); // Initialize serial communication.
		initI2C();	// Initialize I2C communication.
		Framebuffer fb;	// Initialize the OLED screen.
		
		fb.drawBitmap(AE_logo, 64, 128, 0,0);
		fb.show();
		_delay_ms(2000);
		
		uint8_t data[5] = {255, 255, 57, 84, 1};
		
		// Power LoRa().
		// MOSFET/Multiplexer enabling have not yet been added to this code.
		
		initSPI_master();
		initLoRa();
		printString("\nLoRa initialized\n");
		uint8_t RXData[10] = {0};
		uint8_t RSSI100 = 0;
		uint8_t RSSI10 = 0;
		uint8_t RSSI1 = 0;
		uint8_t timeMinute = 0;
		uint8_t timeSeconds = 0;
		uint8_t workingIconNumber = 0;
		uint8_t gotTime = 0;
		fb.drawBitmap(signalStrength, 64, 128, 0, 0);
		fb.drawBitmap(dash[3], 1, 32, 49, 49);
		fb.show();
		
		// ----- As long as the switch is flipped, test LoRa connection and set the time (once). -----
		while (bit_is_clear(PINC,USER_INTERFACE_SWITCH))
		{
			RXData[2] = 255;						// RSSI set to (-)255 if there is no contact.
			LoRa_send_old(data, 16);				// Send the dummy data.
			printString("\nFinished sending\n");
			LoRa_receive_1200msTimeout(RXData);		// Listen for acknowledgment for 1200 ms.
			RSSI100 = RXData[2]/100;				// Extract the 10^2 digit of RSSI.
			RSSI10 = (RXData[2]%100)/10;			// Extract the 10^1 digit of RSSI.
			RSSI1 = (RXData[2]%10);					// Extract the 10^0 digit of RSSI.
			fb.drawBitmap(numbers[RSSI100], 9, 5, 58, 45);	// Draw the 10^2 digit of RSSI.
			fb.drawBitmap(numbers[RSSI10], 9, 5, 66, 45);	// Draw the 10^1 digit of RSSI.
			fb.drawBitmap(numbers[RSSI1], 9, 5, 74, 45);	// Draw the 10^0 digit of RSSI.
			fb.drawBitmap(working[workingIconNumber], 32, 32, 98, 32);
			fb.show();								// Update the screen.
			
			timeMinute = RXData[3];
			timeSeconds = RXData[4];
			
			// If we have not gotten the time yet, and there is connection, and the minutes and seconds has a...
			// ...valid value (just and extra insurance), set the time equal to the time received.
			if(gotTime==0 && RXData[2]<140 && timeMinute<60 && timeSeconds < 60){
				setTime(2019, 7, 18, 16, timeMinute, timeSeconds);
				printString("\nTime set!\n");
				gotTime = 1;
			}
			
			if(gotTime==1){
				printString("\nMinutes now: ");
				printByte(timeNowMinutes());
				printString("\nSeconds now: ");
				printByte(timeNowSeconds());
			}
			
			workingIconNumber++;
			if(workingIconNumber>=4){workingIconNumber=0;} // There are four "spinning wheel" images.
				
			_delay_ms(1000);						// Delay 1000 ms to save battery.
		}
		
		// ----- Send a message to tell the gateway to stop listening, and enter normal operation. -----
		printString("\nBelow here\n");
		fb.drawBitmap(stop_listening, 64, 128, 0, 0);
		fb.show();
		_delay_ms(1000); // Wait an extra 1 s to be sure not to receive an old acknowledgment from the gateway.
		uint8_t timesSent = 0;
		RXData[2] = 255;						// RSSI set to (-)255 if there is no contact.
		data[3] = 123; // data[3]==123 (the "flag" byte of data) is the indication the gateway needs.
		while(timesSent < 3 && RXData[2] == 255){
			LoRa_send_old(data, 16);  // Send the data.
			printString("\nFinished sending out of loop\n");
			LoRa_receive_1200msTimeout(RXData);
			timesSent++;
		}
		printString("\nOut of the loop\n");
		fb.drawBitmap(stopped_listening, 64, 128, 0, 0);
		fb.show();
		_delay_ms(2000);



		// --------------- User unputs current water height. ----------------------------
		DDRB &= 0b01111111; PORTB |= 0b10000000; _delay_ms(10); // Input pullup.
		DDRB |= 0b01000000; PORTB &= 0b10111111; _delay_ms(10); // PB6 Output LOW.
		DDRD &= 0b10011111; PORTD |= 0b01100000; _delay_ms(10);
		


		fb.clear();
		fb.drawBitmap(depth_calibration, 64, 128, 0, 0);
			fb.drawBitmap(numbers[0], 9, 5, 71, 41);
			fb.drawBitmap(numbers[0], 9, 5, 80, 41);
			fb.drawBitmap(numbers[0], 9, 5, 89, 41);
			fb.drawBitmap(numbers[0], 9, 5, 98, 41);
			fb.drawBitmap(dash[3], 1, 32, 71, 53);
		fb.show();
		
		int aVal = 0;
		int aValLast = 0;
		int i[4] = {0};
		int ii = 3;
		int dash_i = 3;
		int isCalibrating = true;
		
		while(isCalibrating)
		{			
			aVal = (bit_is_clear(PINB,PINB7));
			if(aVal != aValLast){
				
				if(bit_is_clear(PINB,PINB7)){
					
					if((bit_is_clear(PIND,PIND5)) != aVal){
						if(i[ii] != 0){
							i[ii]--;
							fb.drawBitmap(numbers[i[3]], 9, 5, 71, 41);
							fb.drawBitmap(numbers[i[2]], 9, 5, 80, 41);
							fb.drawBitmap(numbers[i[1]], 9, 5, 89, 41);
							fb.drawBitmap(numbers[i[0]], 9, 5, 98, 41);
							fb.drawBitmap(dash[dash_i], 1, 32, 71, 53);
							fb.show();
							_delay_ms(5);
						}
					}
					else{
						if(i[ii] != 9){
							i[ii]++;
							fb.drawBitmap(numbers[i[3]], 9, 5, 71, 41);
							fb.drawBitmap(numbers[i[2]], 9, 5, 80, 41);
							fb.drawBitmap(numbers[i[1]], 9, 5, 89, 41);
							fb.drawBitmap(numbers[i[0]], 9, 5, 98, 41);
							fb.drawBitmap(dash[dash_i], 1, 32, 71, 53);
							fb.show();
							_delay_ms(5);
						}
					
					}	
				}
			}
			aValLast = aVal;
			
			if(bit_is_clear(PIND,PIND6)){
				while(bit_is_clear(PIND,PIND6)){}
				if(ii == 0){
					isCalibrating = false;
					depthCompensation = (uint16_t)(i[3]*1000) + (uint16_t)(i[2]*100) + i[1]*10 + i[0];
				}
				else{
					ii--;
					dash_i--;
					fb.drawBitmap(dash[dash_i], 1, 32, 71, 53);
					fb.show();
					_delay_ms(200);
				}
			}
		}
		
		
		
		// ---------- Calculate depth compensation number and sign based on user input and sensor data. -------------
		fb.drawBitmap(wait_depth, 64, 128, 0, 0);
		fb.show();
		uint8_t usePlussOperatorForDepthCompensation; // Make this a global variable and use it in the detDepth() function.
		if(depthCompensation > depth){
			depthCompensation = depthCompensation - depth;
			usePlussOperatorForDepthCompensation = 1;
		}
		else{
			depthCompensation = depth - depthCompensation;
			usePlussOperatorForDepthCompensation = 0;
		}
		_delay_ms(3000); // Simulated measuring, remove later.
		fb.drawBitmap(done_depth, 64, 128, 0, 0);
		fb.show();
		_delay_ms(2000);
		

		fb.drawBitmap(goodbye, 64, 128, 0, 0);
		fb.show();
		while(1){};
		
	
	}
	else{	// The switch is not pulled, the MCU has reset itself. Do nothing. Go straight to event loop.
		initUSART(UBBRValue);
		printString("It's not pulled");
	
	}
	
	
	  powerClockOn();
	  clearAlarm1();
	  powerClockOff();
	  
	  initUSART(UBBRValue);
	  powerGpsMosfetOn();
	  getGPSData(&year, &month, &date, &hour, &minute, &second, latitude, longitude);
	  powerGpsMosfetOff();
	  
	  
	  initI2C();
	  powerClockOn();
	  setTime(year, month, date, hour, minute, second);
	  
	  setAlarm(timeNowMinutes(), timeNowSeconds()+5);
	  

	// ---------------- Event loop. ---------------------------------
	// ---------------- This part of the code still requires A LOT of work. ---------
	while (1) 
{
/*
fakeWakeup = 1;
while(fakeWakeup == 1){
		
	//////// Set pin configuration before sleep ////////
	PRR = 0xff; // Shut off all modules.
	
	DDRB = 0b00000000; //[7] [6] [5]SCK [4]MISO [3]MOSI [2]LoRaSS [1]DepthData [0]
	DDRC = 0b00111001; //[7](nothing) [6]reset [5]SCL [4]SDA [3]clockPower [2] [1] [0]GPSMOSFET
	DDRD = 0b11100000; //[7]LoRaMOSFET [6]DepthMultiplexer [5]DepthMOSFET [4]LoRaInterrupt [3] [2]ClockInterrupt [1]TX [0]RX
	
	PORTB = 0b11111111; //[7] [6] [5]SCK [4]MISO [3]MOSI [2]LoRaSS [1]DepthData [0]
	PORTC = 0b11000110; //[7](nothing) [6]reset [5]SCL [4]SDA [3]clockPower [2] [1] [0]GPSMOSFET
	PORTD = 0b00011111; //[7]LoRaMOSFET [6]DepthMultiplexer [5]DepthMOSFET

	//////// Enable interrupt on clock interrupt pin ////////
	//EICRA = 0x00; // Choose what type sense to trigger interrupt. Here a LOW level is choosen.
	EIMSK |= (1<<INT0); // Enable interrupt on INT0 (pin 4 = PD2).
	sei();

	//////// Go to sleep ////////
	goToSleep();
		
	EIMSK = 0x00;
	cli();
	minimizePowerWhileAwake();
	initUSART(UBBRValue);
	printString("\nYou've been interrupted\n");
	initI2C();
	powerClockOn();
	uint8_t alarmMinute = alarm1Minutes();
	uint8_t alarmSecond = alarm1Seconds();
	clearAlarm1();
		
	// The alarm went off at the right time.
	if(alarmSecond == timeNowSeconds()){
		fakeWakeup = 0;
		setAlarm(timeNowMinutes(), timeNowSeconds()+5);
		powerClockOff();
	}
	
	// False alarm
	else{
		printString("\nFalse alarm");
		setAlarm(alarmMinute, alarmSecond);
		powerClockOff();
		_delay_ms(1);
	}
}
	
	
printString("\nNow at the bottom of the loop\n");
_delay_ms(500);

	


_delay_ms(5000);
depthMosfetOn();
_delay_ms(5000);
depthMultiplexerOn();
initUSART(UBBRValue);
getDepth(&depthMSB, &depthLSB);
depthMultiplexerOff();
depthMosfetOff();
printString("\nSDI Response: ");
printByte(depthMSB);
printString("  ");
printByte(depthLSB);



_delay_ms(1000);
powerClockOn();
seeTimeNow();
powerClockOff();
printString("\n");

printString("Temperature: ");
powerClockOn();
printByte(getTemperature());
printString("\n");
powerClockOff();
_delay_ms(5);


powerLoRaMosfetOn(); 
 initUSART(UBBRValue);
 initSPI_master();
 initLoRa();
 printString("\nLoRa initialized\n");

if(LoRa_sendAndGetAcknowledgement(Sender_Address, Receiver_Address, packageNumber, year, month, date, hour, minute, second, latitude, longitude, temperature, depthMSB, depthLSB, 18, &correctTimeSecond)){
	powerLoRaMosfetOff();
	printString("\ncorrectTimeSecond: ");
	printByte(correctTimeSecond);
	printString("\nKlock after: ");
	powerClockOn();
	setTimeSecond(correctTimeSecond);
	printByte(timeNowSeconds());
	powerClockOff();
	sendFromEEPROM(Sender_Address, Receiver_Address, latitude, longitude, 18, &correctTimeSecond);
}
else{
	powerLoRaMosfetOff();
	printString("\nSending failed");
	storeMessageInEEPROM(packageNumber, year, month, date, hour, minute, second, temperature, depthMSB, depthLSB);
}

for(int i=0; i<10; i++){
	
	uint8_t fraEEPROM = eeprom_read_byte(EEPROM_address + 100 + i);
	printString("\n");
	printByte(fraEEPROM);
}


*/
}	
return 0;}



// When in power down mode, external interrupt, TWI, WDT, 

// IMPORTANT: Set SCL and SDA as output LOW before sleep to reduce power. Save ca. 0.3 mA.
// All LoRa pins (except lora mosfet gate pin) must be set as input high before sleep.
// IMPORTANT: Use EIMSK = 0x00; and cli(); at top of ISR to stop multiple interrupts. 
	//Activate interrupts again with EIMSK |= (1<<INT0); and sei(); before sleep.
// IMPORTANT: Set PRR = 0xff before sleep to deactivate stuff. Remember to initialize stuff again before usage.

  
