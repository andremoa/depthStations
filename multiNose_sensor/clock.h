/*
 * clock.h
 *
 * Created: 12.11.2018 13.12.26
 *  Author: Andreas
 */ 


#ifndef CLOCK_H_
#define CLOCK_H_

#define DS3231_adress_W 0b11010000
#define DS3231_adress_R 0b11010001
#define secondRegister 0x00
#define minuteRegister 0x01
#define hourRegister 0x02
#define dateRegister 0x04
#define monthAndCenturyRegister 0x05
#define yearRegister 0x06
#define A1SecondRegister 0x07
#define A1MinuteRegister 0x08
#define A1HourRegister 0x09
#define A1DateRegister 0x0a
#define clockControlRegister1 0x0e
#define clockControlRegister2 0x0f
#define temperatureIntegerRegister 0x11
#define temperatureFractionRegister 0x12

uint8_t timeNowSeconds(void);
uint8_t timeNowMinutes(void);
uint8_t timeNowHours(void);
uint8_t timeNowDate(void);
uint8_t timeNowMonth(void);
uint8_t timeNowYear(void);
void setTime(uint8_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second);
void setAlarm(uint8_t minute, uint8_t second);
void clearAlarm1(void);
uint8_t getTemperature(void);
void seeTimeNow(void);
void setAralmNext15Minute(int minuteNow, int secondsPast);
uint8_t alarm1Seconds(void);
uint8_t alarm1Minutes(void);
void setTimeSecond(uint8_t second);

#endif /* CLOCK_H_ */