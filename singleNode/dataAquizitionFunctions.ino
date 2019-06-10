// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------- Data data aquizition functions  --------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ---------- Read temperature from the BMP280 pressure and temeprature sensor. --------------------------------------------
uint32_t readBMP280Temperature(uint8_t I2CAddress){

  // --- Give the module time to power up. ---
  delay(50); 

  // --- Register addresses. Found in the BMP280's datasheet. ---
  uint8_t REG_DIG_T1 = 0x88;
  uint8_t REG_DIG_T2 = 0x8A;
  uint8_t REG_DIG_T3 = 0x8C;
  uint8_t REG_CONTROL = 0xF4;
  uint8_t REG_TEMPDATA = 0xFA;

  // --- Factory programmed calibration values, used to calibrate sensor temeperature data. ---
  uint16_t dig_T1 = I2CRead16bits_LE(I2CAddress, REG_DIG_T1);
  int16_t dig_T2 = I2CRead16bits_LE(I2CAddress, REG_DIG_T2);
  int16_t dig_T3 = I2CRead16bits_LE(I2CAddress, REG_DIG_T3);

  // --- Set temperature oversampling of x1 [7:5], skip pressure measurement [4:2] and set power mode to Normal Mode [1:0]. ---
  I2CWrite8bits(I2CAddress, REG_CONTROL, 0b00100011);

  // --- Variables used in the calibration calculation. ---
  int32_t var1, var2;

  // --- Allow time for measurements to be taken (necessary delay found empirically). ---
  delay(400);

  // --- Read the raw temperature data from the sensor (not calibrated). ----
  int32_t adc_T = I2CRead24bits(I2CAddress, REG_TEMPDATA);

  // --- Use the calibration formula found in the datasheet to calculate the correct temperature. ---
  adc_T >>= 4;
  var1  = ((((adc_T>>3) - ((int32_t)dig_T1 <<1))) * ((int32_t)dig_T2)) >> 11;
  var2  = (((((adc_T>>4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
  int32_t t_fine = var1 + var2;
  uint32_t T  = (t_fine * 5 + 128) >> 8;

  // --- Return the correct temperature. ---
  return T;
  
} // End readBMP280Temperature(). 





// ---------- Gets the depth data from the OTT PLS depth sensor. ----------------------------------------------------
uint16_t getDepth(void){

  // --- The commands to be sendt to the sensor. ---
  String startMeasurements = "?M!";
  String getData = "?D0!";
  String SDI_Response = "";
  String SDI_Response_Stripped = "";

  // --- Dummy data. ---
  uint16_t depth = 321;
  
  // --- The sensor needs 5 seconds to power up. ---
  delay(5000);

  // --- Initiate the SDI12 protocol ---
  mySDI12.begin();

  // --- Do measurements. ---
  mySDI12.sendCommand(startMeasurements); // Command to start measurements.
  delay(3000);                            // Wait for it to complete the measurements.
  mySDI12.sendCommand(getData);           // Command to transmit the measurements.
  delay(300);                             // Give time to start transmiting.

  // --- While the sensor sends data, biuld a string from the response.
  while (mySDI12.available()) {
        char c = mySDI12.read();
        SDI_Response += c;
        delay(5);
      }

  // --- Check for corrupted data. ---
  // --- There should be two, and only two '+' or '-' in the string. ---
  // --- Find the position of these two symbols and use them to get the depth data. ---
  // --- See correct data format in OTT PLS' data sheet. ---
  int numberOfPlus = 0;
  int plusOnePosition = 0;
  int plusTwoPosition = 0;
  for(int i=0; i<SDI_Response.length(); i++){
    if(SDI_Response[i] == '+' || SDI_Response[i] == '-'){
      if(numberOfPlus == 0)
      {
        plusOnePosition = i;
        numberOfPlus++;
      }
      else if(numberOfPlus == 1)
      {
        plusTwoPosition = i;
        numberOfPlus++;
      }
      else
      {
        numberOfPlus++;
      }
      
      }
    }

  // --- If there are only two '+' or '-', and the numbers of characters between them are correct... ---
  // --- ... extract the depth data and convert it to uint16_t. ---
  if(numberOfPlus == 2 && (plusTwoPosition-plusOnePosition) == 6)
  {
    SDI_Response_Stripped += SDI_Response[plusOnePosition+1];
    SDI_Response_Stripped += SDI_Response[plusOnePosition+3];
    SDI_Response_Stripped += SDI_Response[plusOnePosition+4];
    SDI_Response_Stripped += SDI_Response[plusOnePosition+5];
    depth = SDI_Response_Stripped.toInt();
  }

  // --- If not, return a number that is out of range for the sensor to show that something is wrong. ---
  else
  {
    return 50000;
  }
  
  // --- Clear the respons string and the buffer. ---
  SDI_Response = "";
  SDI_Response_Stripped = "";
  mySDI12.clearBuffer();

  // --- Return the depth data. ---
  return depth;
  
} // End getDepth().





// ---------- Read the battery voltage. ----------------------------------------------------------------
uint16_t getBatteryVoltage()
{
    // --- Convert the 10-bit analoge read to the true voltage using the voltage dividers resistor values. ---
    uint16_t V_BAT = (uint16_t)((ADC_AREF / 1.023) * (BATVOLT_R1 + BATVOLT_R2) / BATVOLT_R2 * (float)analogRead(BAT_VOLT));

    // --- Return the true battery voltage. ---
    return V_BAT;
    
} // End getBatteryVoltage().





// ---------- Stores the GPS data in flash memory once it has contact with 6 satellites. ----------
// ---------- The user can abort the process and store the data sooner if desired. ----------
uint16_t getGPSData(void){

    // --- Return 0 if the GPS won't initialize properly. ---
     if (myGPS.begin() == false)
    {
      return 0;
    }

    // --- Output UBX only via I2C, not NMEA. Save the settings. ---
    myGPS.setI2COutput(COM_TYPE_UBX);
    myGPS.saveConfiguration();

    // --- Initialize the SSD1306 display. ---
    ssd1306_128x64_i2c_init();
    ssd1306_fillScreen( 0x00 );

    // --- Show information about the GPS procedure to the user. ---
    ssd1306_drawBitmap(0, 0, 128, 64, GPS_info);
    delay(7000);

    // --- Initialize variables and give them dummy values. ---
    int sats = 3;
    int satIconNumber = 1;

    // --- Show the number of connected satellites. ---
    ssd1306_drawBitmap(0, 0, 128, 64, GPS_connection);
    ssd1306_drawBitmap(96, 0, 32, 32, sat_icons[satIconNumber]);

    // --- Must set this one high when reading A0 not to interfere with the read. ---
    // --- Must then be set high again not to interfere with other I2C devices. ---
    // --- See note 1 in notes.h ---
    digitalWrite(TEMPERATURE_POWER_GROUND, HIGH);
    delay(5);

    // --- While contact with less than six satellites, and the user hasn't aborted. ---
    // --- Six satellites gives an accurate enough position (< 10 meter radius). ---
    while(sats < 6 && (!digitalRead(A0))){
      digitalWrite(TEMPERATURE_POWER_GROUND, LOW);
      delay(5);

      // --- Check how many connected sats, update sat number and sat icon on the display. ---
      sats = myGPS.getSIV();
      ssd1306_drawBitmap(82, 5, 11, 16, numbers[sats]);
      ssd1306_drawBitmap(96, 0, 32, 32, sat_icons[satIconNumber]);
      satIconNumber = (satIconNumber+1)%4;
      delay(1000);

      // --- See note 1 in notes.h. ---
      digitalWrite(TEMPERATURE_POWER_GROUND, HIGH);
      delay(5);
    }

    // --- GPS now has contact with six satellites, or the user has aborted. ---
    digitalWrite(TEMPERATURE_POWER_GROUND, LOW);
    delay(5);
    ssd1306_drawBitmap(0, 0, 128, 64, GPS_saving);
    delay(6000); // Let the GPS get a few more datas with 6 satellites.

    // --- Store the GPS coordinates in flash memory. ---
    uint32_t latitude = myGPS.getLatitude();
    uint32_t longitude = myGPS.getLongitude();
    EEPROMUpdate32bits(10, latitude);
    EEPROMUpdate32bits(20, longitude);
    EEPROM.commit();

   // --- Task completed, return 1. ---
    return 1;
    
} // End getGPSData().




//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------- End data aquizition functions. ----------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
