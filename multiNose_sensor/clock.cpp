
#include <avr/io.h>
#include "I2C.h"
#include "clock.h"
#include "USART.h"


uint8_t timeNowSeconds(void){
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(secondRegister);
	i2cStart();
	i2cSend(DS3231_adress_R);
	uint8_t second = i2cReadNoAck();
	i2cStop();
	second = ((second>>4)*10) + (second &= 0b00001111);
	return second;
}

uint8_t timeNowMinutes(void){
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(minuteRegister);
	i2cStart();
	i2cSend(DS3231_adress_R);
	uint8_t minute = i2cReadNoAck();
	i2cStop();
	minute = ((minute>>4)*10) + (minute &= 0b00001111);
	return minute;
}

uint8_t timeNowHours(void){
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(hourRegister);
	i2cStart();
	i2cSend(DS3231_adress_R);
	uint8_t hour = i2cReadNoAck();
	i2cStop();
	uint8_t finalHour = 0;
	uint8_t _hour = hour;
	
	// Check if the 20 hour bit is high, if so, add 20 hours
	if(_hour &= (1<<5)){
		finalHour += 20;
	}
	
	_hour = hour;
	
	// Check if the 10 hour bit is high, if so, add 10 hours
	if(_hour &= 1<<4){
		finalHour += 10;
	}
	
	// Add the remaining hours
	finalHour += (hour &= 0b00001111);
	
	return finalHour;
}

uint8_t timeNowDate(void){
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(dateRegister);
	i2cStart();
	i2cSend(DS3231_adress_R);
	uint8_t date = i2cReadNoAck();
	i2cStop();
	
	date = ((date>>4)*10) + (date &= 0b00001111);
	return date;
}

uint8_t timeNowMonth(void){
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(monthAndCenturyRegister);
	i2cStart();
	i2cSend(DS3231_adress_R);
	uint8_t month = i2cReadNoAck();
	i2cStop();
	
	month &= 0b01111111; // Remove the century bit.
	month = ((month>>4)*10) + (month &= 0b00001111);
	return month;
}

uint8_t timeNowYear(void){
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(yearRegister);
	i2cStart();
	i2cSend(DS3231_adress_R);
	uint8_t year = i2cReadNoAck();
	i2cStop();
	
	year = ((year>>4)*10) + (year &= 0b00001111);
	return year;
}

void setTime(uint8_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second){
		
	uint8_t setYear = 0b00000000;
	if(year >= 50){
		setYear = 0b01010000;
		year -= 50;
	}
	else if(year >= 40){
		setYear = 0b01000000;
		year -= 40;
	}
	else if(year >= 30){
		setYear = 0b00110000;
		year -= 30;
	}
	else if(year >= 20){
		setYear = 0b00100000;
		year -= 20;
	}
	else if(year >= 10){
		setYear = 0b00010000;
		year -= 10;
	}
	// Add the remaining single years
	setYear += year;
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(yearRegister);
	i2cSend(setYear);
	i2cStop();
	
	
	uint8_t setMonth = 0b10000000; // Set century bit to 1.
	if(month >= 10){
		setMonth = 0b10010000;
		month -= 10;
	}
	setMonth += month;
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(monthAndCenturyRegister);
	i2cSend(setMonth);
	i2cStop();
	
	
	uint8_t setDate = 0b00000000;
	if(date >= 30){
		setDate = 0b00110000;
		date -= 30;
	}
	else if(date >= 20){
		setDate = 0b00100000;
		date -= 20;
	}
	else if(date >= 10){
		setDate = 0b00010000;
		date -= 10;
	}
	setDate += date;
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(dateRegister);
	i2cSend(setDate);
	i2cStop();
	
	
	uint8_t setHour = 0b00000000; // Bit 6 must be set low to enter 24 hour mode.
	// Set the 20 hour bit.
	if(hour >= 20){
		setHour |= (1<<5);
		hour -= 20;
	}
	//Set the 10 hour bit
	else if(hour >= 10){
		setHour |= (1<<4);
		hour -= 10;
	}
	// Add the remaining hours
	setHour += hour;
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(hourRegister);
	i2cSend(setHour);
	i2cStop();
	
	
	uint8_t setMinute = 0b00000000;
	if(minute >= 50){
		setMinute = 0b01010000;
		minute -= 50;
	}
	else if(minute >= 40){
		setMinute = 0b01000000;
		minute -= 40;
	}
	else if(minute >= 30){
		setMinute = 0b00110000;
		minute -= 30;
	}
	else if(minute >= 20){
		setMinute = 0b00100000;
		minute -= 20;
	}
	else if(minute >= 10){
		setMinute = 0b00010000;
		minute -= 10;
	}
	// Add the remaining single minutes
	setMinute += minute;
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(minuteRegister);
	i2cSend(setMinute);
	i2cStop();
	
	
	uint8_t setSecond = 0b00000000;
	if(second >= 50){
		setSecond = 0b01010000;
		second -= 50;
	}
	else if(second >= 40){
		setSecond = 0b01000000;
		second -= 40;
	}
	else if(second >= 30){
		setSecond = 0b00110000;
		second -= 30;
	}
	else if(second >= 20){
		setSecond = 0b00100000;
		second -= 20;
	}
	else if(second >= 10){
		setSecond = 0b00010000;
		second -= 10;
	}
	// Add the remaining single seconds
	setSecond += second;
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(secondRegister);
	i2cSend(setSecond);
	i2cStop();

}

void setAlarm(uint8_t minute, uint8_t second){
	
	uint8_t setSecond = 0b00000000; // Bit 7 must be 0 to set the Alarm Mask Bit right (match condition for minute and second).
	if(second >= 50){
		setSecond = 0b01010000;
		second -= 50;
	}
	else if(second >= 40){
		setSecond = 0b01000000;
		second -= 40;
	}
	else if(second >= 30){
		setSecond = 0b00110000;
		second -= 30;
	}
	else if(second >= 20){
		setSecond = 0b00100000;
		second -= 20;
	}
	else if(second >= 10){
		setSecond = 0b00010000;
		second -= 10;
	}
	// Add the remaining single seconds
	setSecond += second;
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(A1SecondRegister);
	i2cSend(setSecond);
	i2cStop();
	
	uint8_t setMinute = 0b00000000; // Bit 7 must be 0 to set the Alarm Mask Bit right (match condition for minute and second).
	if(minute >= 50){
		setMinute = 0b01010000;
		minute -= 50;
	}
	else if(minute >= 40){
		setMinute = 0b01000000;
		minute -= 40;
	}
	else if(minute >= 30){
		setMinute = 0b00110000;
		minute -= 30;
	}
	else if(minute >= 20){
		setMinute = 0b00100000;
		minute -= 20;
	}
	else if(minute >= 10){
		setMinute = 0b00010000;
		minute -= 10;
	}
	// Add the remaining single minutes
	setMinute += minute;
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(A1MinuteRegister);
	i2cSend(setMinute);
	i2cStop();
	
	uint8_t setHour = 0b10000000;
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(A1HourRegister);
	i2cSend(setHour); // We don't care about the hour, but bit 7 of this register must be set to 1 to set the Alarm Mask Bit right.
	i2cStop(); // Is this stop needed?
	
	uint8_t setDate = 0b10000000;
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(A1DateRegister);
	i2cSend(setDate); // We don't care about the date, but bit 7 of this register must be set to 1 to set the Alarm Mask Bit right.
	i2cStop();
	
	// Activate interrupt output when Alarm 1 Flag is set (A1F is set automatically when the time equals the alarm).
	i2cStart();
	i2cSend(DS3231_adress_R);
	i2cSend(clockControlRegister1);
	uint8_t controlRegister = i2cReadNoAck();
	i2cStop();
	controlRegister |= 0b01000101; // Set INTCN (bit 2) and A1IE (bit 0) without touching the other bits.
	controlRegister &= 0b01111111; // Set EOSC (bit 7) to zero to keep oscillator on even without Vcc (only on Vbat).
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(clockControlRegister1);
	i2cSend(controlRegister);
	i2cStop();
	
}

void clearAlarm1(void){
	// Clear Alarm 1 Flag in Control Register 2 without touching the other bits in the register.
	i2cStart();
	i2cSend(DS3231_adress_R);
	i2cSend(clockControlRegister2);
	uint8_t controlRegister = i2cReadNoAck();
	i2cStop();
	controlRegister &= 0b11111100; // A1F is bit 0.
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(clockControlRegister2);
	i2cSend(controlRegister);
	i2cStop();
}

uint8_t getTemperature(void){
	uint8_t getTemp = 0;
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(temperatureIntegerRegister);
	i2cStart();
	i2cSend(DS3231_adress_R);
	getTemp = i2cReadNoAck();
	i2cStop();
	return getTemp;

}

void seeTimeNow(void){
	printByte(timeNowSeconds());
	printString("\n");
	printByte(timeNowMinutes());
	printString("\n");
	printByte(timeNowHours());
	printString("\n");
	printByte(timeNowDate());
	printString("\n");
	printByte(timeNowMonth());
	printString("\n");
	printByte(timeNowYear());
	printString("\n");
	
}



void setAralmNext15Minute(int minuteNow, int secondsPast){
	if(minuteNow < 15){
		setAlarm(15, secondsPast);
	}
	
	else if(minuteNow < 30){
		setAlarm(30, secondsPast);
	}
	
	else if(minuteNow < 45){
		setAlarm(45, secondsPast);
	}
	
	else{
		setAlarm(0, secondsPast);
	}
}





uint8_t alarm1Seconds(void){
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(A1SecondRegister);
	i2cStart();
	i2cSend(DS3231_adress_R);
	uint8_t second = i2cReadNoAck();
	i2cStop();
	second = ((second>>4)*10) + (second &= 0b00001111);
	return second;
}


uint8_t alarm1Minutes(void){
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(A1MinuteRegister);
	i2cStart();
	i2cSend(DS3231_adress_R);
	uint8_t minute = i2cReadNoAck();
	i2cStop();
	minute = ((minute>>4)*10) + (minute &= 0b00001111);
	return minute;
}


void setTimeSecond(uint8_t second){
	uint8_t setSecond = 0b00000000;
	if(second >= 50){
		setSecond = 0b01010000;
		second -= 50;
	}
	else if(second >= 40){
		setSecond = 0b01000000;
		second -= 40;
	}
	else if(second >= 30){
		setSecond = 0b00110000;
		second -= 30;
	}
	else if(second >= 20){
		setSecond = 0b00100000;
		second -= 20;
	}
	else if(second >= 10){
		setSecond = 0b00010000;
		second -= 10;
	}
	// Add the remaining single seconds
	setSecond += second;
	i2cStart();
	i2cSend(DS3231_adress_W);
	i2cSend(secondRegister);
	i2cSend(setSecond);
	i2cStop();
}