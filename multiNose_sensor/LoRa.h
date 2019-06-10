/*
 * LoRa.h
 *
 * Created: 17.11.2018 15.37.45
 *  Author: Andreas
 */ 


#ifndef LORA_H_
#define LORA_H_


// LoRa Defines
#define LoRa_WRITE_MASK 0b10000000 // The first bit of the adress must be a 1 to enable write.
#define LoRa_SENDER_ADDRESS 1
#define LoRa_RECEIVER_ADDRESS 2
#define LoRa_LORA_MODE 0x80
#define LoRa_SLEEP_MODE 0x00 // Change between LoRa and FSK/OOK mode can only happen in sleep mode.
#define LoRa_STANDBY_MODE 0b00000001 // Keep the MSB 1 to keep the chip in LoRa mode?
#define LoRa_TRANSMIT_MODE 0b00000011 // MSB is still 1 (LoRa mode) just to be sure?
#define LoRa_SINGLE_RECEIVE_MODE 0b10000110
#define LoRa_CONTINUOUS_RECEIVE_MODE 0b00000101
#define LoRa_FXOSC 32E6 // Oscillator frequency of the chip (32 MHz).
#define LoRa_PACKAGE_LENGTH 10
#define LoRa_FIFO_RX_BASE 0x00
#define LoRa_FIFO_TX_BASE 0x80
#define LoRa_DIO0_REGISTER DDRD
#define LoRa_DIO0_STATUS PIND
#define LoRa_DIO0_PORT 0b00010000
#define LoRa_DIO0_IS_TxDone 0b01000000
#define LoRa_DIO0_IS_RxDone 0x00
#define LoRa_PA_DAC_ENABLE 0x07
#define LoRa_PA_SELECT 0b10000000


#define RegFifo_0x00 0x00
#define RegFifoAddrPtr_0x0D 0x0d
#define RegFifoTxBaseAddr_0x0E 0x0e
#define RegFifoRxBaseAddr_0x0F 0x0f
#define RegFifoRxCurrentAddr_0x10 0x10 // Start address (in data buffer) of last packet received.
#define RegRxNbBytes_0x13 0x13 // Number of payload bytes of last received package.
#define RegOpMode_0x01 0x01
#define RegPaConfig_0x09 0x09
#define RegPreambleMsb_0x20 0x20
#define RegPreambleLsb_0x21 0x21
#define RegModemConfig1_0x1D 0x1d
#define RegModemConfig2_0x1E 0x1e
#define RegModemConfig3_0x26 0x26
#define RegIrqFlags_0x12 0x12
#define RegFrMsb_0x06 0x06
#define RegFrMid_0x07 0x07
#define RegFrLsb_0x08 0x08
#define RegPayloadLength_0x22 0x22
#define RegDioMapping1_0x40 0x40
#define RegPktRssiValue_0x1A 0x1a
#define RegPktSnrValue_0x19 0x19
#define RegPaDac_0x4D 0x4d // PA_BOOST pin register

#define F_CPU 8000000L

void initLoRa(void);
void SPI_writeToLoRaRegister(uint8_t address, uint8_t data);
int LoRa_send_old(uint8_t* data, uint8_t length);
int LoRa_receive(uint8_t* buf);
int LoRa_receive_1200msTimeout(uint8_t* buf);
int LoRa_send(uint8_t* Sender_Address, uint8_t* Receiver_Address, uint8_t packetNumber, uint8_t* year, uint8_t* month, uint8_t* date, uint8_t* hour, uint8_t* minute, uint8_t* second, uint8_t* latitude, uint8_t* longitude, uint8_t* temperature, uint8_t* depthMSB, uint8_t* depthLSB, uint8_t length);
int LoRa_sendAndGetAcknowledgement(uint8_t* Sender_Address, uint8_t* Receiver_Address, uint8_t* packetNumber, uint8_t* year, uint8_t* month, uint8_t* date, uint8_t* hour, uint8_t* minute, uint8_t* second, uint8_t* latitude, uint8_t* longitude, uint8_t* temperature, uint8_t* depthMSB, uint8_t* depthLSB, uint8_t length);
int LoRa_send_confirmation(uint8_t senderAddress, uint8_t receiverAddress, uint8_t length);
int LoRa_receiveAndSendConfirmation(uint8_t* buf, uint8_t senderAddress, uint8_t receiverAddress);
int LoRa_sendAndGetAcknowledgement(uint8_t Sender_Address, uint8_t Receiver_Address, uint8_t packetNumber, uint8_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second, uint8_t* latitude, uint8_t* longitude, uint8_t temperature, uint8_t depthMSB, uint8_t depthLSB, uint8_t length, uint8_t* correctTimeSecond);
void storeMessageInEEPROM(uint8_t packageNumber, uint8_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second, uint8_t temperature, uint8_t depthMSB, uint8_t depthLSB);
void sendFromEEPROM(uint8_t Sender_Address, uint8_t Receiver_Address, uint8_t* latitude, uint8_t* longitude, uint8_t length, uint8_t* correctTimeSecond);

#endif /* LORA_H_ */