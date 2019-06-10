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


// --- Use these two arrays and three #defines below instead of the..
// ...defines above, once the new Telenor server is ready. ---
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


// The SODAQ board has several USART ports.
// Use "SerialUSB" for all that otherwise is labled "Serial".
// This must be done before including other files.
#define Serial SerialUSB


// ----- Includes --------------------------------------------------------------------
#include "sam.h"              // Board specific definitions.
#include "portDefines.h"      // 
#include <RTCZero.h>          // Internal RTC functions.
#include <SDI12.h>            // SDI-12 protocol.
#include "globalVariables.h"  // Global variables and definitions.
#include "ssd1306.h"          // Display functions.
#include "bitPatterns.h"      // Bit patterns for the SSD1306 display.
#include <FlashAsEEPROM.h>    // Include EEPROM-like API for FlashStorage.
#include <SPI.h>              // SPI protocol.
#include <SD.h>               // SD card functions.
#include <Sodaq_LSM303AGR.h>  // Accelerometer and gyroscope handling.
#include "SparkFun_Ublox_Arduino_Library.h" // GPS functions.
// ------------------------------------------------------------------------------------


// ----- Object instanciation ---------------------------------------------------------
File myFile;                    // Create an instance of an SD card object.
Sodaq_LSM303AGR accelerometer;  // Create an instance of an accelerometer&gyroscope object.
SFE_UBLOX_GPS myGPS;            // Create an instance for a GPS object.
RTCZero rtc;                    // Create an instance of the rtc object.
SDI12 mySDI12(SDI12_DATA_PIN);  // Define the SDI-12 bus.





// ---------------------------------------------------------------------------------------------------------------------
// ------------------------ Main code setup. Will run once -------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
void setup() {
  SerialUSB.begin(9600); // Initialize the USB serial debug line (to print to the monitor).
  
  // --- Power up all peripheral I2C so they don't keep the bus busy. ---
  pinMode(DISPLAY_POWER_MOSFET,OUTPUT); 
  digitalWrite(DISPLAY_POWER_MOSFET, HIGH);     // Power the display.
  pinMode(TEMPERATURE_POWER_GROUND, OUTPUT); 
  digitalWrite(TEMPERATURE_POWER_GROUND, LOW);  // Power the temperature sensor.
  
  // --- Power down the microSD reader, and set all SPI pins HIGH. ---
  pinMode(SD_CARD_POWER_MOSFET, OUTPUT); 
  digitalWrite(SD_CARD_POWER_MOSFET, LOW);    // Power down the SD card reader.
  pinMode(10,OUTPUT); digitalWrite(10,HIGH);
  pinMode(11,OUTPUT); digitalWrite(11,HIGH);
  pinMode(12,OUTPUT); digitalWrite(12,HIGH);
  pinMode(13,OUTPUT); digitalWrite(13,HIGH);  // 13 = LED_BUILTIN. Remember to remove LED_BUILTIN, which is connected to pin D13. 
  delay(10);
  
  rtc.begin(); // Initialize RTC.
  Wire.begin();// Initialize the I2C protocol.
  
  // --- Disable the LSM303AGR accelerometer&gyroscope and power down the GPS. ---
  accelerometer.disableAccelerometer();
  accelerometer.disableMagnetometer();
  pinMode(MAG_INT,  OUTPUT);
  digitalWrite(MAG_INT, LOW); // To stop current leakage on this pin.
  pinMode(GPS_ENABLE,  OUTPUT);
  digitalWrite(GPS_ENABLE, LOW);

  // --- Disconnect USB. Comment out when debugging/writing new code. ---
  SerialUSB.flush();
  SerialUSB.end();
  USBDevice.detach();
  USB->DEVICE.CTRLA.reg &= ~USB_CTRLA_ENABLE; // Disable USB
  
  // --- Power down the ublox modem ---
  pinMode(SARA_ENABLE, OUTPUT); 
  digitalWrite(SARA_ENABLE, LOW);
  pinMode(SARA_TX_ENABLE, OUTPUT);
  digitalWrite(SARA_TX_ENABLE, LOW);
  
  // --- Power down the depth sensor ---
  pinMode(DEPTH_SENSOR_POWER,OUTPUT);
  digitalWrite(DEPTH_SENSOR_POWER,LOW);
  
  // --- Blink the red LED and turn them all off ---
  pinMode(LED_GREEN,OUTPUT);
  digitalWrite(LED_GREEN,HIGH);
  pinMode(LED_BLUE,OUTPUT);
  digitalWrite(LED_BLUE,HIGH);
  pinMode(LED_RED,OUTPUT);
  digitalWrite(LED_RED,LOW);
  delay(100);
  digitalWrite(LED_RED,HIGH);
  
  // --- Set the MODE SWITHCH pin to input with a pullup ---
  pinMode(A0, INPUT_PULLUP);
  delay(20);
  
  digitalWrite(TEMPERATURE_POWER_GROUND, HIGH); // See note 1 in globalVariables.h.



  // ----- If the MODE SWITHCH is in the NORMAL position, check for signal strength --------------------------------------
  if(digitalRead(A0)){// A0 is HIGH due to internal resistor when in "NORMAL" position.
    
    digitalWrite(TEMPERATURE_POWER_GROUND, LOW); // See note 1 in globalVariables.h.
  
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


  
  digitalWrite(TEMPERATURE_POWER_GROUND, HIGH); // See note 1 in globalVariables.h.



  // ----- If the MODE SWITCH is in the CALIBRATE position, calibrate and get GPS data. ------------------------------------------
  if(!digitalRead(A0)) // A0 is LOW when in "CALIBRATE" position.
  {
    digitalWrite(TEMPERATURE_POWER_GROUND, LOW); // See note 1 in globalVariables.h 
    
    digitalWrite(GPS_ENABLE, HIGH);              // Power the GPS, and start searching for satellites.
    
    // --- Set the encoder pins to required states. ---
    pinMode(ROTATORY_CLK_PIN, INPUT);
    pinMode(ROTATORY_DT_PIN, INPUT);
    pinMode(ROTATORY_SW_PIN, INPUT);
    pinMode(ROTATORY_PLUS,OUTPUT);
    digitalWrite(ROTATORY_PLUS,LOW);

    // --- Initiate display and show the AE logo for 1 second. ---
    ssd1306_128x64_i2c_init();
    ssd1306_fillScreen( 0x00 );
    ssd1306_drawBitmap(0, 0, 128, 64, AE_logo);
    delay(1000);
  
    // --- The user must enter the correct password before being allowed to calibrate the sensor ---
    while(userInputPassword != password){
      userInputPassword = inputPassword();
      if(userInputPassword != password){
        ssd1306_drawBitmap(0, 0, 128, 64, wrongPassword);
        delay(2000); // Show image for two seconds.
      }
    }

    // --- The user here inputs the current depth. ---
    currentDepth = inputCurrentDepthMASL();         

    // --- Tell user to wait while depth measurements are calibrated. ---
    ssd1306_drawBitmap(0, 0, 128, 64, wait_depth);

    // --- Power sensor, get sensor depth, power down sensor. ---
    pinMode(DEPTH_SENSOR_POWER,OUTPUT);
    digitalWrite(DEPTH_SENSOR_POWER,HIGH);
    sensorDepth = getDepth();
    digitalWrite(DEPTH_SENSOR_POWER,LOW);

    // --- Calculate depth compensation number and the sign of the number. ---
    if(currentDepth >= sensorDepth){
      depthCompensation = currentDepth - sensorDepth;
      depthCompensationSign = 1;
    }
    else{
      depthCompensation = sensorDepth - currentDepth;
      depthCompensationSign = 0;
    }

    // --- Store the sign and number in Flash (simulated EEPROM). ---
    EEPROM.update(0, depthCompensationSign);
    EEPROMUpdate32bits(1, depthCompensation);
    EEPROM.commit();    

    // --- Tell the user that the calibration is done. ---
    ssd1306_drawBitmap(0, 0, 128, 64, done_depth);
    delay(2000);

    // --- Get GPS data. See comments in the function for full descriptio. ---
    getGPSData();

    // --- The user must flip the switch back to exit CALIBRATE mode. See comment 2 in note.h. ---
    while(!digitalRead(A0)){
    ssd1306_drawBitmap(0, 0, 128, 64, flip_switch);
    }

    // Tell the user that the program will enter autonomus mode (the event loop). ---
    ssd1306_drawBitmap(0, 0, 128, 64, goodbye);
    delay(3000);
    
  } // End of CALIBRATE mode. ------------------------------------------------------------------

  
  
  // --- Power down stuff and set pin states before entering the event loop. ---
  digitalWrite(GPS_ENABLE, LOW);
  digitalWrite(DISPLAY_POWER_MOSFET, LOW);
  digitalWrite(TEMPERATURE_POWER_GROUND, HIGH);
  pinMode(ROTATORY_CLK_PIN, INPUT_PULLUP);
  pinMode(ROTATORY_DT_PIN, INPUT_PULLUP);
  pinMode(ROTATORY_SW_PIN, INPUT_PULLUP);
  pinMode(ROTATORY_PLUS, INPUT_PULLUP);
  
  // --- Set dummy time and date and alarm to wake up after 10 seconds for the first run. ---
  rtc.setTime(dummyHours, dummyMinutes, dummySeconds);
  rtc.setDate(dummyDay, dummyMonth, dummyYear);
  rtc.setAlarmTime(1,1,10);
  rtc.enableAlarm(rtc.MATCH_HHMMSS);
  rtc.attachInterrupt(alarmMatch);
  
}

// ----------------------------------------------------------------------------------------------------------------------------------
//  ---------------------------- End main code setup(). -----------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------------------------





// ----------------------------------------------------------------------------------------------------------------------------------
// ------------------------ The event loop. Will loop indefinetly -------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------------------------
void loop() {

  // --- Set pin states to reduce current leakage before going to sleep. ---
  pinMode(A4, INPUT_PULLUP); // SDA
  pinMode(A5, INPUT_PULLUP); // SCL
  pinMode(SDI12_DATA_PIN, INPUT_PULLUP); // SDI-12

  
  // --- Go to sleep. Wake up by RTC alarm. ---
  goToSleep();

  
  // --- Get the battery voltage. ---
  batteryVoltage = getBatteryVoltage();

  
  // --- Get the time and date of measurement (from the RTC). ---
  yearNow = rtc.getYear();
  monthNow = rtc.getMonth();
  dateNow = rtc.getDay();
  hoursNow = rtc.getHours();
  minutesNow = rtc.getMinutes();

  
  // --- Power the ublox module and start searching for network. ---
  pinMode(SARA_ENABLE, OUTPUT);
  digitalWrite(SARA_ENABLE, HIGH);
  pinMode(SARA_TX_ENABLE, OUTPUT);
  digitalWrite(SARA_TX_ENABLE, HIGH);
  SerialUBLOX.begin(9600); // Initialize the serial line to the ublox modem.
  connectionStartTime = millis(); // Start time for connection timer.

  
  // --- Power depth sensor, get data, power down. ---
  pinMode(DEPTH_SENSOR_POWER,OUTPUT);
  digitalWrite(DEPTH_SENSOR_POWER,HIGH);
  depth = getDepth();
  digitalWrite(DEPTH_SENSOR_POWER,LOW);


  // --- Adjust depth measurement to compensation number. ---
  depthCompensationSign = EEPROM.read(0);
  depthCompensation = EEPROMRead32bits(1);
  if(depthCompensationSign == 1)
  {
    depth = depth + depthCompensation;
  }
  else
  {
    depth = depth - depthCompensation;
  }

  
  // --- Power all peripheral I2C devices not to hold bus busy. Get temperature. Power down. ---
  digitalWrite(DISPLAY_POWER_MOSFET, HIGH);
  digitalWrite(TEMPERATURE_POWER_GROUND, LOW); 
  temperature = readBMP280Temperature(0x77);
  digitalWrite(DISPLAY_POWER_MOSFET, LOW); 
  digitalWrite(TEMPERATURE_POWER_GROUND, HIGH);

  
  // --- Put latitude and longitude in RAM. ---
  latitude = EEPROMRead32bits(10);
  longitude = EEPROMRead32bits(20);

  
  // ----- If connection is established before timeout: send the data. ------------------------------------
  if(getTime130sTimeout()) // Updates the time if connection is established.
  {
    connectionTime = millis() - connectionStartTime;  // Calculate connection time.
    connectionTime = connectionTime/1000;             // Connection time in seconds, not ms.
    
    // --- Send the data ---
    sendToAllThingsTalk(temperature, connectionTime, depth, batteryVoltage, counter, latitude, longitude, monthNow, dateNow, hoursNow);
    // The function above can be replaced with NBIoTSendData() to send to arbitrary server and port. Use after agreement with Telenor is ready.
    // Combine with convertDataToJSON() and convertStringToASCIIHex() to send in JSON format. 
    sendUnsentData(); // Send unsent data if there are any.
  }

  // ----- If timeout, store data in RAM and mark as unsent. ---------
  else
  {
    connectionTime = 0;
    storeUnsentData();
  }

  // --- Power down the ublox modem. ---
  digitalWrite(SARA_ENABLE, LOW);
  digitalWrite(SARA_TX_ENABLE, LOW);
  
  
  // ----- Write data to the microSD card. ------------------------------
  // --- Make file name equal to the year and month. ---
  String fileName = "";
  fileName = String(yearNow) + String(monthNow) + ".txt";

  // --- Create a string of the data in the correct format. ---
  String dataToSD = "";
  dataToSD = numbersToCSVString(yearNow, monthNow, dateNow, hoursNow, minutesNow, temperature, connectionTime, depth, batteryVoltage, counter, 98765432);

  // --- Power the microSD card and give it time to boot. ---
  digitalWrite(SD_CARD_POWER_MOSFET, HIGH);
  delay(10);

  // -- If the card responds and the file opens: Write the data. ---
  if (SD.begin(10)) {
    myFile = SD.open(fileName, FILE_WRITE);
    if (myFile) {
      myFile.println(dataToSD);
      myFile.close();
    }
  }

  // --- Power down the microSD card and set SPI pin states.
  digitalWrite(SD_CARD_POWER_MOSFET, LOW);
  pinMode(10,OUTPUT); digitalWrite(10,HIGH);
  pinMode(11,OUTPUT); digitalWrite(11,HIGH);
  pinMode(12,OUTPUT); digitalWrite(12,HIGH);
  pinMode(13,OUTPUT); digitalWrite(13,HIGH); 

  
  // --- Update counter ---
  counter++;
  
  // --- Set the next alarm ---
  //setAlarmNext15Minute();
  setAlarmNext15Second();
  
}
// -------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------ Bottom of the event loop. ----------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
