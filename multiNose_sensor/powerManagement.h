/*
 * powerManagement.h
 *
 * Created: 12.11.2018 18.51.37
 *  Author: Andreas
 */ 


#ifndef POWERMANAGEMENT_H_
#define POWERMANAGEMENT_H_

/* Define clock frequency */
#define F_CPU 8000000L

/* Set registers and pin states to use little power */
void minimizePowerWhileAwake(void);

/* Disable all peripherals and go to low-power mode */
void goToSleep(void);

/* Turn on and off power to the clock */
void powerClockOn(void);
void powerClockOff(void);

/* Turn on and off power to the GPS */
void powerGpsMosfetOn(void);
void powerGpsMosfetOff(void);

/* Turn on and off power to the radio driver */
void powerLoRaMosfetOn(void);
void powerLoRaMosfetOff(void);

/* Activate and deactivate the multiplexer for the depth sensor */
void depthMultiplexerOn(void);
void depthMultiplexerOff(void);

/* Turn on and off the power to the depth sensor */
void depthMosfetOn(void);
void depthMosfetOff(void);


#endif /* POWERMANAGEMENT_H_ */