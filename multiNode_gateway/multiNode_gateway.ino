// ----- User input ----------------------------------------------------------------------
#define DeviceID "My5sVHzEqwUYcit057T3DoRu"
#define DeviceToken "maker:4IxXLoJVNIZzG0lqFyyQSHtmNOs3ZSyfO2bDM3Q1"
#define Asset1 "temperature"
#define Asset2 "connectionTime"
#define Asset3 "depth"
#define Asset4 "voltage"
#define Asset5 "counter"
#define Asset6 "latitude"
#define Asset7 "longitude"
#define Asset8 ""
#define Asset9 ""
#define Asset10 ""


// --- Use these two arrays and three #defines instead of the..
// ...defines above once the new Telenor server is ready. ---
// --- Blank asset names ("") will be skipped. ---
// --- ASSETS[] and ASSETS_LOG_SCALE[] is used to build the JSON data. ---
#define NodeID "HS_IoT5"
#define SERVER_IP_ADDRESS ""
#define SERVER_PORT 1234

const char* ASSETS[] = 
{
    "temperature[degC]",            // Asset1
    "connectionTime[s]",            // Asset2
    "depth[m]",                     // Asset3
    "voltage[V]",                   // Asset4
    "counter",                      // Asset5
    "latitude",                     // Asset6
    "longitude",                    // Asset7
    "date",                         // Asset8
    "time",                         // Asset9
    "altitude",                     // Asset10
    "music",
    "TeSt"
};

const char* ASSETS_LOG_SCALE[] = 
{
    "18",             // Asset1
    "-19",            // Asset2
    "20",             // Asset3
    "21",             // Asset4
    "22",             // Asset5
    "23",             // Asset6
    "24",             // Asset7
    "25",             // Asset8
    "-3",             // Asset9
    "5",              // Asset10
};
// ----------------------------------------------------------------------------------------


#include "sam.h"
#include "portDefines.h"
#include <RTCZero.h>
#include "globalVariables.h"
#include "ssd1306.h"
#include "bitPatterns.h"
#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>
#include "SparkFun_Ublox_Arduino_Library.h" // GPS functions.
#include <FlashAsEEPROM.h>    // Include EEPROM-like API for FlashStorage.

#include <Sodaq_LSM303AGR.h>
Sodaq_LSM303AGR accelerometer;

#define CLIENT_ADDRESS 255
#define SERVER_ADDRESS 255
#define Serial SerialUSB

// Create an instance of the rtc object.
RTCZero rtc;

// Create an instance for a GPS object
SFE_UBLOX_GPS myGPS;    
        
// Singleton instance of the radio driver
RH_RF95 driver (4,3);

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);


/* -------------------- Setup -------------------- */
void setup() {

SerialUSB.begin(9600); // Initialize the USB serial debug line (to print to the monitor).

pinMode(DISPLAY_POWER_MOSFET,OUTPUT);
digitalWrite(DISPLAY_POWER_MOSFET, HIGH); // The display uses I2C, and must therefore be active in order not to interfere with the LSM303AGR who also uses I2C.
Wire.begin();// I2C for the accelerometer
// Disable the LSM303AGR
accelerometer.disableAccelerometer();
accelerometer.disableMagnetometer();
pinMode(LED_BUILTIN,  OUTPUT);
pinMode(MAG_INT,  OUTPUT);
pinMode(GPS_ENABLE,  OUTPUT);
digitalWrite(LED_BUILTIN, LOW);   // led low=off, high=on
digitalWrite(MAG_INT, LOW);       // we need to make this low otherwise this pin on the LSM303AGR starts leaking current
digitalWrite(GPS_ENABLE, LOW);    // low=poweredoff, high=poweredon

/* Power down the UBLOX module */
pinMode(SARA_ENABLE, OUTPUT); 
digitalWrite(SARA_ENABLE, LOW);
pinMode(SARA_TX_ENABLE, OUTPUT);
digitalWrite(SARA_TX_ENABLE, LOW);

// ----- Blink red led and turn off all leds -----
pinMode(LED_GREEN,OUTPUT);
digitalWrite(LED_GREEN,HIGH);
pinMode(LED_BLUE,OUTPUT);
digitalWrite(LED_BLUE,HIGH);
pinMode(LED_RED,OUTPUT);
digitalWrite(LED_RED,LOW);
delay(100);
digitalWrite(LED_RED,HIGH);


// ----- Initialize RTC -----
rtc.begin(); 

// --- Power the ublox module and start searching for network ---
digitalWrite(SARA_ENABLE, HIGH);
digitalWrite(SARA_TX_ENABLE, HIGH);
SerialUBLOX.begin(9600); // Initialize the serial line to the UBLOX module (NB-IoT).
digitalWrite(LED_RED,LOW);
delay(1000); // Must give the u-blox module some time to boot before sending it commands.

// --- Wait for connection and then get the time stamp ---
getTheTime(); // Can use this to also check for connection. Doesn't update the time before it's connected.
digitalWrite(LED_RED,HIGH);
digitalWrite(LED_GREEN,LOW);

// --- Powering of radio and display via MOSFET has not yet been added to this code. ---

// --- Set mode switch as input. ---
pinMode(A0, INPUT);


// ----- If the MODE SWITHCH is in the NORMAL position, check for signal strength --------------------------------------
if(digitalRead(A0)){// A0 is HIGH due to internal resistor when in "NORMAL" position.
  

  // --- Power the ublox module and start searching for network ---
  pinMode(SARA_ENABLE, OUTPUT); 
  digitalWrite(SARA_ENABLE, HIGH);
  pinMode(SARA_TX_ENABLE, OUTPUT);
  digitalWrite(SARA_TX_ENABLE, HIGH);
  
  SerialUBLOX.begin(9600); // Initialize the serial line to the UBLOX module (NB-IoT).

  // --- Initiate the display and show the "Searching" image ---
  ssd1306_128x64_i2c_init();
  ssd1306_fillScreen( 0x00 );
  ssd1306_drawBitmap(0, 0, 128, 64, searching);
  
  delay(1000);          // Give the ublox modem some time to power up (required).
  getTheTime();            // Wait for connection.
  showSignalStrength(); // Show the signal strength on the display.
  
  // --- Power down the SARA module ---
  digitalWrite(SARA_ENABLE, LOW);
  digitalWrite(SARA_TX_ENABLE, LOW);
  
  delay(5000); // Give the user some time to see the signal strength.
  
} // ----- End of NORMAL mode. Set pin states, then goes straight to event loop. ----------------------------------------------











uint32_t timeNow = 0;
uint32_t timeLastMessage = millis();
uint32_t timeElapsed = 0;


if(!digitalRead(A0)){ // User input - deploy mode.

  // --- Initialize LoRa -----
  manager.init();

  // When the sensor node is finished checking the signal, it sends the "random" number 123.
  while(flag != 123 && timeElapsed < 3600000){
    
    if (manager.available())
      {
        Serial.println("Is available");
        // Wait for a message addressed to us from the client
        uint8_t len = sizeof(buf);
        uint8_t from;
        uint8_t to;
        uint8_t id;        
        if (manager.recvfromAck(buf, &len, &from, &to, &id, &flag, 1, 1))
        {
          timeLastMessage = millis();
          manager.acknowledge(1,2,rtc.getMinutes(),rtc.getSeconds());
          Serial.print("Got request from sender ID: ");
          Serial.println(from);
          Serial.print("To: ");
          Serial.println(to);
          Serial.print("ID: ");
          Serial.println(id);
          Serial.print("Flag: ");
          Serial.println(flag);
          Serial.print("Buf: ");
          Serial.println((char*)buf);
          Serial.print("To: ");
          Serial.println(to);
          Serial.print("RSSI: ");
          Serial.println(driver.lastRssi(), DEC);
          uint8_t test = buf[0];
          Serial.println(test);
          SerialUSB.print("rtc.getMinutes() = ");
          SerialUSB.println(rtc.getMinutes());
          SerialUSB.print("rtc.getSeconds() = ");
          SerialUSB.println(rtc.getSeconds());
        }
      }

    // If more than 1 hour have passed since last message, stop listening.
    timeNow = millis();
    timeElapsed = timeNow - timeLastMessage;
   }

timeLastMessage = millis();
timeNow = millis();
timeElapsed = 0;
// Listen for 10 more seconds in case the previous acknowledgement wasn't received.
  while(timeElapsed < 10000){
    
    if (manager.available())
      {
        Serial.println("Is available");
        // Wait for a message addressed to us from the client
        uint8_t len = sizeof(buf);
        uint8_t from;
        uint8_t to;
        uint8_t id;        
        if (manager.recvfromAck(buf, &len, &from, &to, &id, &flag, 1, 1))
        {
          manager.acknowledge(1,2,rtc.getMinutes(),rtc.getSeconds());
        }
      }

    // If more than 1 hour have passed since last message, stop listening.
    timeNow = millis();
    timeElapsed = timeNow - timeLastMessage;
   }

   
SerialUSB.println("Out of the loop");
// --- Set the next alarm ---
//setAlarmNext15Minute();
setAlarmNext15Second(); // Used for testing while programming.
//alarmNowMinutes = rtc.getMinutes();
//alarmNowSeconds = rtc.getSeconds();
SerialUSB.print(rtc.getMinutes());
SerialUSB.print(":");
SerialUSB.println(rtc.getSeconds());
}

}//End setup().




/* -------------------- Event loop -------------------- */
// --------------- This part of the code for the gateway have not been worked on yet. ------------------
void loop() {

/* Go to sleep */
USBDevice.detach();
rtc.standbyMode();
USBDevice.attach();
  // Wait for serial USB port to open
  while(!SerialUSB);
  // Serial USB is blazing fast, you might miss the messages
  delay(1000);

for(int x=0; x<10; x++){
  digitalWrite(LED_RED,HIGH);
  digitalWrite(LED_BLUE,LOW);
  delay(500);
   digitalWrite(LED_RED,LOW);
  digitalWrite(LED_BLUE,HIGH);
  delay(500);
}

  SerialUSB.print(rtc.getMinutes());
  SerialUSB.print(":");
  SerialUSB.println(rtc.getSeconds());
  
digitalWrite(LED_RED,HIGH);
digitalWrite(LED_BLUE,LOW);
SerialUSB.begin(9600);
SerialUSB.println("HERE");

/* Get battery voltage */
batteryVoltage = getBatteryVoltage();

// --- Power the ublox module and start searching for network ---
pinMode(SARA_ENABLE, OUTPUT);
digitalWrite(SARA_ENABLE, HIGH);
pinMode(SARA_TX_ENABLE, OUTPUT);
digitalWrite(SARA_TX_ENABLE, HIGH);
SerialUBLOX.begin(9600); // Initialize the serial line to the UBLOX module (NB-IoT).
connectionStartTime = millis();
digitalWrite(LED_RED,LOW);
delay(1000);

// --- Start listening for a radio message. ---
manager.init();
LoRa_listenStartTime = millis();
LoRa_listenTime = 0;
flag = 0;
while(LoRa_listenTime < 30 || flag == 0){
  if (manager.available())
      {
        Serial.println("Is available");
        // Wait for a message addressed to us from the client
        uint8_t len = sizeof(buf);
        uint8_t from;
        uint8_t to;
        uint8_t id;        
        if (manager.recvfromAck(buf, &len, &from, &to, &id, &flag, 3, 4))
        {
          manager.acknowledge(1,2,rtc.getMinutes(),rtc.getSeconds());
          Serial.println("Got message");
          SerialUSB.print("rtc.getMinutes() = ");
  SerialUSB.println(rtc.getMinutes());
  SerialUSB.print("rtc.getSeconds() = ");
  SerialUSB.println(rtc.getSeconds());
        }
      }
}

// --- Wait for connection and then get the time ---
getTheTime(); // Can use this to also check for connection. Doesn't update the time before it's connected.
connectionTime = millis() - connectionStartTime;
digitalWrite(LED_RED,HIGH);
digitalWrite(LED_GREEN,LOW);

SerialUSB.print("Connection time: ");
SerialUSB.print(connectionTime);
while(1){}

// --- Send the data ---
//sendDataToAllthingstalk(depth, batteryVoltage, counter, connectionTime);
digitalWrite(LED_GREEN,HIGH);

// --- Power down the SARA module ---
digitalWrite(SARA_ENABLE, LOW);
digitalWrite(SARA_TX_ENABLE, LOW);

// --- Set the next alarm ---
setAlarmNext15Minute();
//digitalWrite(LED_BLUE,HIGH);
//digitalWrite(LED_RED,LOW);


}// End event loop.
