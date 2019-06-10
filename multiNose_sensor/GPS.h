/*
 * GPS.h
 *
 * Created: 12.11.2018 12.26.40
 *  Author: Andreas
 */ 


#ifndef GPS_H_
#define GPS_H_

void getGPSData(uint8_t* year, uint8_t* month, uint8_t* date, uint8_t* hour, uint8_t* minute, uint8_t* second, uint8_t* latitude, uint8_t* longitude);
uint8_t convertFromASCIIToBinary(uint8_t input);


#endif /* GPS_H_ */