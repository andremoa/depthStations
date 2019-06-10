
#include <avr/io.h>
#include "SPI.h"
#include "LoRa.h"
#include "USART.h"
#include <util/delay.h>
#include "I2C.h"
#include "clock.h"
#include "powerManagement.h"
#include <avr/eeprom.h>




void initLoRa(void){
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_SLEEP_MODE); // The RFM chip must be in sleep mode in order to change to LoRa mode.
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_LORA_MODE); // Set chip to LoRa mode (not FSK/OOK mode). Must be done while in sleep mode.
	SPI_writeToLoRaRegister(RegFifoTxBaseAddr_0x0E, 0); // Set both the FIFO transmit...
	SPI_writeToLoRaRegister(RegFifoRxBaseAddr_0x0F, 0); // .. and the FIFO receive address base to zero.
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_STANDBY_MODE); // Set the chip to standby mode.
	
	
	// Set [7-4]bandwidth, [3-1]coding rate and [0]header mode.
	SPI_writeToLoRaRegister(RegModemConfig1_0x1D, 0x78); // Bandwidth = 125 kHz. Coding rate = 4/8. Header mode = explicit.
	
	
	// Set [7-4]spreading factor, [3]Packet mode, [2]CRC, [1-0](9-8)symbol timeout.
	SPI_writeToLoRaRegister(RegModemConfig2_0x1E, 0xc4); // Spreading factor = 4096 chips/symbol. Packet mode = single. CRC = enabled. Symbol timeout = default.
	
	// Set [7-4]Reserved, [3]LowDataRateOptimize, [2]AgcAutoOn, [1-0]Reserved.
	SPI_writeToLoRaRegister(RegModemConfig3_0x26, 0x0c); // LowDataRateOptimize = optimize. AgcAutoOn = LNA gain set by the internal AGC loop.
	
	// Set preamble length to default of 12 (8+4) symbols. Range from 6 to 65535.
	SPI_writeToLoRaRegister(RegPreambleMsb_0x20, 0x00);
	SPI_writeToLoRaRegister(RegPreambleLsb_0x21, 0x08); 
	
	// Set frequency to ~870.1 MHz.
	SPI_writeToLoRaRegister(RegFrMsb_0x06, 0xd9); // The formula is: Frequency = (FXOSC * inputParameter) / 2^19.
	SPI_writeToLoRaRegister(RegFrMid_0x07, 0x86); // FXOSC = 32E6. Here inputParameter is 0xD98666.
	SPI_writeToLoRaRegister(RegFrLsb_0x08, 0x66); // See the previous two lines.
	
	// Enable maximum transmit power.
	SPI_writeToLoRaRegister(RegPaDac_0x4D, LoRa_PA_DAC_ENABLE); // Enable +20 dBm capability on PA_BOOST pin.
	SPI_writeToLoRaRegister(RegPaConfig_0x09, LoRa_PA_SELECT | 15); // PA_BOOST + maximum transmitting power.

}

// Same as SPI_writeToRegister, but it adds the LoRa_WRITE_MASK when selecting register.
void SPI_writeToLoRaRegister(uint8_t address, uint8_t data){
	SS_SELECT;
	SPI_tradeByte(address | LoRa_WRITE_MASK);
	SPI_tradeByte(data);
	SS_DESELECT;
}




int LoRa_send_old(uint8_t* data, uint8_t length){
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_STANDBY_MODE);
	
	SPI_writeToLoRaRegister(RegFifoAddrPtr_0x0D, 0);
	
	for(int i=0; i<length; i++){
		SPI_writeToLoRaRegister(RegFifo_0x00, *data);
		data++;
	}
	
	SPI_writeToLoRaRegister(RegPayloadLength_0x22, length);
	
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_TRANSMIT_MODE);
	SPI_writeToLoRaRegister(RegDioMapping1_0x40, 0x40); // Interrupt on TxDone
	
	// Wait until transmission is done or skip if there is an error (timeout). Fix later!
	uint16_t msUsed = 0;
	while((LoRa_DIO0_STATUS & LoRa_DIO0_PORT) == 0 && (msUsed < 3000)){
		msUsed++;
		_delay_ms(1);
	}
	
	if(msUsed >= 2999){
		printString("\nDid not send\n");
	}
	
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_STANDBY_MODE);
	
	SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	
	return 1;
}

int LoRa_receive(uint8_t* buf){
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_CONTINUOUS_RECEIVE_MODE);
	SPI_writeToLoRaRegister(RegDioMapping1_0x40, 0x00); // Interrupt on RxDone
	
	while((LoRa_DIO0_STATUS & LoRa_DIO0_PORT) == 0){} // Wait until a message is received. (Not needed in single receive mode?)
	
	uint8_t length = SPI_readFromRegister(RegRxNbBytes_0x13);
	
	SPI_writeToLoRaRegister(RegFifoAddrPtr_0x0D, SPI_readFromRegister(RegFifoRxCurrentAddr_0x10));
	
	for(int i=0; i < length; i++){
		buf[i] = (uint8_t)SPI_readFromRegister(RegFifo_0x00);
	}
	
	SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_STANDBY_MODE);
	
	int CRC_status = SPI_readFromRegister(RegIrqFlags_0x12); // Read the IRQ flag registers to get the PayloadCrcError.
	// Check if there is a payload CRC error (bit 5 in the register). (Can also be activated as an interrupt on DIO3).
	if((CRC_status & 0b00100000) == 0b00100000){
		printString("\nCRC error\n");
	}
	else{
		printString("\nNo CRC error\n");
	}
	
	SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	
	printString("length: ");
	printByte(length);
	printString("\n");
	
	for(int i=0; i<length; i++){
		printByte(buf[i]);
	}
	
	return 1;
}


int LoRa_receive_1200msTimeout(uint8_t* buf){
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_CONTINUOUS_RECEIVE_MODE);
	SPI_writeToLoRaRegister(RegDioMapping1_0x40, 0x00); // Interrupt on RxDone
	
	// Wait until a message is received, or waiting time runs out.
	uint16_t timeSpent = 0;
	while((LoRa_DIO0_STATUS & LoRa_DIO0_PORT) == 0 && (timeSpent < 1200)){
		_delay_ms(1);
		timeSpent++;
	}
	
	int CRC_status = SPI_readFromRegister(RegIrqFlags_0x12); // Read the IRQ flag registers to get the PayloadCrcError.
	// Check if there is a payload CRC error (bit 5 in the register). (Can also be activated as an interrupt on DIO3).
	//if((CRC_status & 0b00100000) == 0b00100000){
	//	printString("\nCRC error\n");
	//}
	//else{
	//	printString("\nNo CRC error\n");
	//}

	// If there has been an "message received" interrupt and there are no CRC errors, we are interested in the message.
	if((LoRa_DIO0_STATUS & LoRa_DIO0_PORT) != 0	&&	(CRC_status & 0b00100000) != 0b00100000){
		uint8_t length = SPI_readFromRegister(RegRxNbBytes_0x13);
	
		SPI_writeToLoRaRegister(RegFifoAddrPtr_0x0D, SPI_readFromRegister(RegFifoRxCurrentAddr_0x10));
	
		for(int i=0; i < length; i++){
			buf[i] = (uint8_t)SPI_readFromRegister(RegFifo_0x00);
		}
	
		SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	
		SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_STANDBY_MODE);
	
		SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
		SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	
		printString("length: ");
		printByte(length);
		printString("\n");
	
		for(int i=0; i<length; i++){
			printByte(buf[i]);
		}
	
		return 1;
	}
	else{return 0;}
}


// Length = 17 bytes
int LoRa_send(uint8_t Sender_Address, uint8_t Receiver_Address, uint8_t packetNumber, uint8_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second, uint8_t* latitude, uint8_t* longitude, uint8_t temperature, uint8_t depthMSB, uint8_t depthLSB, uint8_t length){
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_STANDBY_MODE);
	
	SPI_writeToLoRaRegister(RegFifoAddrPtr_0x0D, 0);
	
	SPI_writeToLoRaRegister(RegFifo_0x00, Sender_Address);
	SPI_writeToLoRaRegister(RegFifo_0x00, Receiver_Address);
	SPI_writeToLoRaRegister(RegFifo_0x00, packetNumber);
	SPI_writeToLoRaRegister(RegFifo_0x00, year);
	SPI_writeToLoRaRegister(RegFifo_0x00, month);
	SPI_writeToLoRaRegister(RegFifo_0x00, date);
	SPI_writeToLoRaRegister(RegFifo_0x00, hour);
	SPI_writeToLoRaRegister(RegFifo_0x00, minute);
	SPI_writeToLoRaRegister(RegFifo_0x00, second);
	SPI_writeToLoRaRegister(RegFifo_0x00, latitude[0]);
	SPI_writeToLoRaRegister(RegFifo_0x00, latitude[1]);
	SPI_writeToLoRaRegister(RegFifo_0x00, latitude[2]);
	SPI_writeToLoRaRegister(RegFifo_0x00, longitude[0]);
	SPI_writeToLoRaRegister(RegFifo_0x00, longitude[1]);
	SPI_writeToLoRaRegister(RegFifo_0x00, longitude[2]);
	SPI_writeToLoRaRegister(RegFifo_0x00, temperature);
	SPI_writeToLoRaRegister(RegFifo_0x00, depthMSB);
	SPI_writeToLoRaRegister(RegFifo_0x00, depthLSB);
	
	SPI_writeToLoRaRegister(RegPayloadLength_0x22, length);
	
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_TRANSMIT_MODE);
	SPI_writeToLoRaRegister(RegDioMapping1_0x40, 0x40); // Interrupt on TxDone
	
	while((LoRa_DIO0_STATUS & LoRa_DIO0_PORT) == 0){} // Wait until transmission is done.
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_STANDBY_MODE);
	
	SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	
	return 1;
}





int LoRa_receive_confirmation(uint8_t* confirmationMessage, uint8_t senderAddress, uint8_t receiverAddress, uint8_t* correctTimeSecond){
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_CONTINUOUS_RECEIVE_MODE);
	SPI_writeToLoRaRegister(RegDioMapping1_0x40, 0x00); // Interrupt on RxDone
	
	// Wait until a message is received, or waiting time runs out.
	uint16_t timeSpent = 0;
	while((LoRa_DIO0_STATUS & LoRa_DIO0_PORT) == 0 && (timeSpent < 1500)){
		_delay_ms(1);
		timeSpent++;
	}
	
	// Time ran out and no message was received.
	if((LoRa_DIO0_STATUS & LoRa_DIO0_PORT) == 0){
		SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
		SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
		SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_STANDBY_MODE);
		return 0;
	}
	
	printString("\n");
	printWord(timeSpent);
	printString("\n");
	
	// If we get here, a message was received. But is it our confirmation?
	SPI_writeToLoRaRegister(RegFifoAddrPtr_0x0D, SPI_readFromRegister(RegFifoRxCurrentAddr_0x10));
	
	for(int i=0; i < 3; i++){
		confirmationMessage[i] = (uint8_t)SPI_readFromRegister(RegFifo_0x00);
	}
	
	SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_STANDBY_MODE);
	
	printString("\nHere:");
	for(int i=0; i<3;i++){
		_delay_ms(5);
		printByte(confirmationMessage[i]);
		printString("\n");
	}
	
	/*
	int CRC_status = SPI_readFromRegister(RegIrqFlags_0x12); // Read the IRQ flag registers to get the PayloadCrcError.
	// Check if there is a payload CRC error (bit 5 in the register). (Can also be activated as an interrupt on DIO3).
	if((CRC_status & 0b00100000) == 0b00100000){
		printString("\nCRC error\n");
	}
	else{
		printString("\nNo CRC error\n");
	}
	*/
	
	SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	
	if(confirmationMessage[0] == receiverAddress && confirmationMessage[1] == senderAddress){
		*correctTimeSecond = confirmationMessage[2];
		return 1;
	}
	
	
	// We got a message, but it wasn't the right one.
	return 0;
}


int LoRa_sendAndGetAcknowledgement(uint8_t Sender_Address, uint8_t Receiver_Address, uint8_t packetNumber, uint8_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second, uint8_t* latitude, uint8_t* longitude, uint8_t temperature, uint8_t depthMSB, uint8_t depthLSB, uint8_t length, uint8_t* correctTimeSecond){
	uint8_t attempts = 0;
	while(attempts++ < 3){
		LoRa_send(Sender_Address, Receiver_Address, packetNumber, year, month, date, hour, minute, second, latitude, longitude, temperature, depthMSB, depthLSB, length);
		
		uint8_t confirmationMessage[3];
		if(LoRa_receive_confirmation(confirmationMessage, Sender_Address, Receiver_Address, correctTimeSecond)){
			printString("\nSent, and received confirmation\n");
			return 1;
		}
	}
	printString("\nCouldn't send\n");
	// If we get here, no confirmation has been received.
	return 0;
}









int LoRa_send_confirmation(uint8_t receiverAddress, uint8_t senderAddress, uint8_t length){
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_STANDBY_MODE);
	
	SPI_writeToLoRaRegister(RegFifoAddrPtr_0x0D, 0);
	
	SPI_writeToLoRaRegister(RegFifo_0x00, receiverAddress);
	SPI_writeToLoRaRegister(RegFifo_0x00, senderAddress);
	
	initI2C();
	powerClockOn();
	uint8_t secondNow = timeNowSeconds();
	powerClockOff();
	SPI_writeToLoRaRegister(RegFifo_0x00, secondNow);
	
	SPI_writeToLoRaRegister(RegPayloadLength_0x22, length);
	
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_TRANSMIT_MODE);
	SPI_writeToLoRaRegister(RegDioMapping1_0x40, 0x40); // Interrupt on TxDone
	
	while((LoRa_DIO0_STATUS & LoRa_DIO0_PORT) == 0){} // Wait until transmission is done.
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_STANDBY_MODE);
	
	SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	
	return 1;
}


int LoRa_receiveAndSendConfirmation(uint8_t* buf, uint8_t senderAddress, uint8_t receiverAddress){
	SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_CONTINUOUS_RECEIVE_MODE);
	SPI_writeToLoRaRegister(RegDioMapping1_0x40, 0x00); // Interrupt on RxDone
	
	while((LoRa_DIO0_STATUS & LoRa_DIO0_PORT) == 0){} // Wait until a message is received. (Not needed in single receive mode?)
	
	//uint8_t length = SPI_readFromRegister(RegRxNbBytes_0x13);
	
	SPI_writeToLoRaRegister(RegFifoAddrPtr_0x0D, SPI_readFromRegister(RegFifoRxCurrentAddr_0x10));
	
	for(int i=0; i < 18; i++){
		buf[i] = (uint8_t)SPI_readFromRegister(RegFifo_0x00);
	}
	
	//SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	
	//SPI_writeToLoRaRegister(RegOpMode_0x01, LoRa_STANDBY_MODE);
	
	/*
	int CRC_status = SPI_readFromRegister(RegIrqFlags_0x12); // Read the IRQ flag registers to get the PayloadCrcError.
	// Check if there is a payload CRC error (bit 5 in the register). (Can also be activated as an interrupt on DIO3).
	if((CRC_status & 0b00100000) == 0b00100000){
		printString("\nCRC error\n");
	}
	else{
		printString("\nNo CRC error\n");
	}
	*/
	
	SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	SPI_writeToLoRaRegister(RegIrqFlags_0x12, 0xff); // Clear all IRQ flags
	
	if(buf[0] == senderAddress && buf[1] == receiverAddress){
		//printString("\nGot correct data");
		LoRa_send_confirmation(receiverAddress, senderAddress, 3);
	}
	
	return 1;
}



void storeMessageInEEPROM(uint8_t packageNumber, uint8_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second, uint8_t temperature, uint8_t depthMSB, uint8_t depthLSB){
	uint8_t *baseAddress = (uint8_t *) 0; // The address must be a pointer. Use this as a base and add to to it to get the correct address.
	
	eeprom_update_byte(baseAddress + 100 + packageNumber*20 + 0, packageNumber); // No need to store this one, but its good for reference.
	eeprom_update_byte(baseAddress + 100 + packageNumber*20 + 1, year);
	eeprom_update_byte(baseAddress + 100 + packageNumber*20 + 2, month);
	eeprom_update_byte(baseAddress + 100 + packageNumber*20 + 3, date);
	eeprom_update_byte(baseAddress + 100 + packageNumber*20 + 4, hour);
	eeprom_update_byte(baseAddress + 100 + packageNumber*20 + 5, minute);
	eeprom_update_byte(baseAddress + 100 + packageNumber*20 + 6, second);
	eeprom_update_byte(baseAddress + 100 + packageNumber*20 + 7, temperature);
	eeprom_update_byte(baseAddress + 100 + packageNumber*20 + 8, depthMSB);
	eeprom_update_byte(baseAddress + 100 + packageNumber*20 + 9, depthLSB);
}


void sendFromEEPROM(uint8_t Sender_Address, uint8_t Receiver_Address, uint8_t* latitude, uint8_t* longitude, uint8_t length, uint8_t* correctTimeSecond){
	uint8_t *baseAddress = (uint8_t *) 0; // The address must be a pointer. Use this as a base and add to to it to get the correct address.
	
	// Go through all the potential package numbers. Maximum 10 stored packages (can be changed).
	for(uint8_t packageNumber=0; packageNumber<10; packageNumber++){
		
		uint8_t pn = packageNumber*20; // The multiplying sign '*' was interpreted as a pointer symbol inside the read function.
		
		uint8_t month = eeprom_read_byte(baseAddress + 100 + pn + 2);
		
		// The month byte in EEPROM is set to zero if the message have been sent from EEPROM, or if no message ever have been written there.
		if(month == 0){}
		else{
			uint8_t packageNumber = eeprom_read_byte(baseAddress + 100 + pn + 0);
			uint8_t year = eeprom_read_byte(baseAddress + 100 + pn + 1);
			uint8_t date = eeprom_read_byte(baseAddress + 100 + pn + 3);
			uint8_t hour = eeprom_read_byte(baseAddress + 100 + pn + 4);
			uint8_t minute = eeprom_read_byte(baseAddress + 100 + pn + 5);
			uint8_t second = eeprom_read_byte(baseAddress + 100 + pn + 6);
			uint8_t temperature = eeprom_read_byte(baseAddress + 100 + pn + 7);
			uint8_t depthMSB = eeprom_read_byte(baseAddress + 100 + pn + 8);
			uint8_t depthLSB = eeprom_read_byte(baseAddress + 100 + pn + 9);
			
			if(LoRa_sendAndGetAcknowledgement(Sender_Address, Receiver_Address, packageNumber, year, month, date, hour, minute, second, latitude, longitude, temperature, depthMSB, depthLSB, length, correctTimeSecond)){
				eeprom_update_byte(baseAddress + 100 + pn + 2, 0);
				printString("\nMessage sent from EEPROM");
			}
		}
	}
}