// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------Alarm functions ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ----- Sets the alarm the next whole 15 minute -----
void setAlarmNext15Minute(void){
  
  // --- Check the time now to know when to set the next alarm.
  uint8_t minuteNow = rtc.getMinutes();
  uint8_t hourNow = rtc.getHours();

  // --- Find the next whole 15 minute and set the alarm. ---
  if(minuteNow < 15){
    rtc.setAlarmTime(hourNow,15,0);
  }
  else if(minuteNow < 30){
    rtc.setAlarmTime(hourNow,30,0);
  }
  else if(minuteNow < 45){
    rtc.setAlarmTime(hourNow,45,0);
  }  
  else{
    if(hourNow < 23){ // Be aware of a new hour.
      rtc.setAlarmTime(hourNow+1,0,0);
    }
    else{ // Be aware of a new day.
      rtc.setAlarmTime(0,0,0);
    }
  }

  // Trigger alarm when hour, minute and seconds of the RTC match the set alarm.
  rtc.enableAlarm(rtc.MATCH_HHMMSS);

  // Attach an interrupt to the alarm.
  rtc.attachInterrupt(alarmMatch);
  
} // End setAlarmNext15Minute().





// ----- The same as setAlarmNext15Minute(), except the alarm triggers the next whole 15 second. -----
// ----- Useful for debugging, to speed things up. -----
void setAlarmNext15Second(void){
  uint8_t secondNow = rtc.getSeconds();
  uint8_t minuteNow = rtc.getMinutes();
  uint8_t nextMinute = minuteNow + 1;
  uint8_t hourNow = rtc.getHours();

  if(secondNow < 15){
    rtc.setAlarmTime(hourNow,minuteNow,15);
  }
  else if(secondNow < 30){
    rtc.setAlarmTime(hourNow,minuteNow,30);
  }
  else if(secondNow < 45){
    rtc.setAlarmTime(hourNow,minuteNow,45);
  }
  else{
    if(minuteNow < 59){
      rtc.setAlarmTime(hourNow,nextMinute,00);
    }
    else{
      rtc.setAlarmTime(hourNow+1,nextMinute,00);
    }
  }

  rtc.enableAlarm(rtc.MATCH_HHMMSS);
  rtc.attachInterrupt(alarmMatch);

} // End setAlarmNext15Second().





// ----- The function that is set to run once when the interrupt is triggered. -----
void alarmMatch(){
  rtc.disableAlarm(); // Disable the alarm.
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------- End alarm functions -----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------








// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------Data conversion and data processing functions. --------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ----- Converts a single ASCII digit into an actual binary number. ----- 
uint8_t convertFromASCIIToBinary(uint8_t input){
  switch(input){
    case 0b00110000:
    return 0b00000000;
    case 0b00110001:
    return 0b00000001;
    case 0b00110010:
    return 0b00000010;
    case 0b00110011:
    return 0b00000011;
    case 0b00110100:
    return 0b00000100;
    case 0b00110101:
    return 0b00000101;
    case 0b00110110:
    return 0b00000110;
    case 0b00110111:
    return 0b00000111;
    case 0b00111000:
    return 0b00001000;
    case 0b00111001:
    return 0b00001001;
    default:
    return input;
  }
  
} // End convertFromASCIIToBinary().





// ----- Takes a dec number (0-9) and converts it to the corresponding ASCII character represented by its hex number. -----
// ----- Ex: 4(dex) -> 34(hex) = 4(ASCII). -----
String convertDecToASCIIHex(uint8_t input){ 
  switch(input){
    case 0:
    return "30";
    case 1:
    return "31";
    case 2:
    return "32";
    case 3:
    return "33";
    case 4:
    return "34";
    case 5:
    return "35";
    case 6:
    return "36";
    case 7:
    return "37";
    case 8:
    return "38";
    case 9:
    return "39";
    default:
    return "-1";
  }
} // End convertDecToASCIIHex().





// ----- Converts a one digit number into its ASCII representation. -----
String convertDecToASCII(uint8_t input){ 
  switch(input){
    case 0:
    return "0";
    case 1:
    return "1";
    case 2:
    return "2";
    case 3:
    return "3";
    case 4:
    return "4";
    case 5:
    return "5";
    case 6:
    return "6";
    case 7:
    return "7";
    case 8:
    return "8";
    case 9:
    return "9";
    default:
    return "-1";
  }
  
} // End convertDecToASCII().





// ---------- Converts a string to its ASCII hexadecimal reprecentation. ---------------------------------------
String convertStringToASCIIHex(String data){

  // --- Declare an empty sring. ---
  String ASCIIHex = "";

  // --- Convert each element of the character array (string) to its hex representation. ---
  // --- Add it to the string that will be returned. ---
  for(int i=0; i<data.length(); i++){
    ASCIIHex += String(data[i], HEX);
  }

  // --- Retrun the ASCII hex string. ---
  return ASCIIHex;
  
} // End convertStringToASCIIHex().





// ---------- Returns a string in JSON format of the ASSETS (names) assetValue (value) and ASSETS_LOG_SCALE (the logarithmic scale of the value). ---------------------------------
// ---------- If the name of the asset (ASSETS[i]) is empty, i.e. = "", it will skip the asset. -------------------------------------------------------------
String convertDataToJSON(uint32_t asset1Value, uint32_t asset2Value, uint32_t asset3Value, uint32_t asset4Value, uint32_t asset5Value, uint32_t asset6Value, uint32_t asset7Value, uint32_t asset8Value, uint32_t asset9Value, uint32_t asset10Value){

  // --- Create a list of the arguments in order to loop through them later. ---
  uint32_t assetValue[10] = {0};
  assetValue[0] = asset1Value;
  assetValue[1] = asset2Value;
  assetValue[2] = asset3Value;
  assetValue[3] = asset4Value;
  assetValue[4] = asset5Value;
  assetValue[5] = asset6Value;
  assetValue[6] = asset7Value;
  assetValue[7] = asset8Value;
  assetValue[8] = asset9Value;
  assetValue[9] = asset10Value;

  // --- Declare an empty string to be returned later. ---
  String JSONData = "";

  // --- SensorID and preamble. ---
  JSONData = "{'SensorID':'<";
  JSONData += NodeID;
  JSONData += ">','Readings':{";

  // --- Loop thorugh all assets. Skip if the name == "". ---
  // --- If not empty, build the data string. ---
  for(int i=0; i<10; i++){
    if(ASSETS[i] != ""){
      JSONData += "'";
      JSONData += ASSETS[i];
      JSONData += "':{'value':";
      JSONData += String(assetValue[i]);
      JSONData += ",'logscale':";
      JSONData += ASSETS_LOG_SCALE[i];
      JSONData += "},";
    }
  }

  // --- Suffix of the JSON string. ---
  JSONData += "}}";

  // --- Return the JSON string. ---
  return JSONData;
  
} // End convertDataToJSON().





// ----- Converts uint32_t numbers to a SVG string. -----
String numbersToCSVString(uint32_t data0, uint32_t data1, uint32_t data2, uint32_t data3, uint32_t data4, uint32_t data5, uint32_t data6, uint32_t data7, uint32_t data8, uint32_t data9, uint32_t data10){
  String output = "";
  
  output = uint32ToPositiveOrNegativeString(data0);
  output += ",";
  output += uint32ToPositiveOrNegativeString(data1);
  output += ",";
  output += uint32ToPositiveOrNegativeString(data2);
  output += ",";
  output += uint32ToPositiveOrNegativeString(data3);
  output += ",";
  output += uint32ToPositiveOrNegativeString(data4);
  output += ",";
  output += uint32ToPositiveOrNegativeString(data5);
  output += ",";
  output += uint32ToPositiveOrNegativeString(data6);
  output += ",";
  output += uint32ToPositiveOrNegativeString(data7);
  output += ",";
  output += uint32ToPositiveOrNegativeString(data8);
  output += ",";
  output += uint32ToPositiveOrNegativeString(data9);
  output += ",";
  output += uint32ToPositiveOrNegativeString(data10);

  // --- Return the SVG string. ---
  return output;
  
} // End numbersToCSVString().





// ---------- Converts a uint32_t to the same number in a character array (String) format. ----------
// ---------- If the number is larger than 41E8, larger than any number used in this project... -----
// ---------- ... it's a negative, and gets a '-' in front of it. ----------
String uint32ToPositiveOrNegativeString(uint32_t number){
  String outputString = "";
  uint32_t negativeNumber = 0;

  // --- If larger than 41E8, convert to 2's complement and string and add '-'. ---
  if(number > 4100000000){
    negativeNumber = 0xffffffff - number + 1;
    outputString = "-" + String(negativeNumber);
  }

  // --- If not, just convert to string. ---
  else{
    outputString = String(number);
  }

  // --- Return the string. ---
  return outputString;
  
} // End uint32ToPositiveOrNegativeString().





// ---------- Converts a uint32_t number to ASCII format represented by its hexadecimal value. ----------
String convertIntToASCIIHex(uint32_t number){

  // --- Declaration of the string to be returned. ---
  String numberHex = "";

  if(number >= 1000000000){
    numberHex = convertDecToASCIIHex(number/1000000000); // Extract the 10^9 digit.
    numberHex += convertDecToASCIIHex(number/100000000 - (number/1000000000)*10); // Extract the 10^8 digit.
    numberHex += convertDecToASCIIHex(number/10000000 - (number/100000000)*10); // Extract the 10^7 digit.
    numberHex += convertDecToASCIIHex(number/1000000 - (number/10000000)*10); // Extract the 10^6 digit.
    numberHex += convertDecToASCIIHex(number/100000 - (number/1000000)*10); // Extract the 10^5 digit.
    numberHex += convertDecToASCIIHex(number/10000 - (number/100000)*10); // Extract the 10^4 digit.
    numberHex += convertDecToASCIIHex(number/1000 - (number/10000)*10); // Extract the 10^3 digit.
    numberHex += convertDecToASCIIHex(number/100 - (number/1000)*10); // Extract the 10^2 digit.
    numberHex += convertDecToASCIIHex(number/10 - (number/100)*10); // Extract the 10^1 digit.
    numberHex += convertDecToASCIIHex(number - (number/10)*10); // Extract the 10^0 digit.
  }
  
  else if(number >= 100000000){
    numberHex = convertDecToASCIIHex(number/100000000); // Extract the 10^8 digit.
    numberHex += convertDecToASCIIHex(number/10000000 - (number/100000000)*10); // Extract the 10^7 digit.
    numberHex += convertDecToASCIIHex(number/1000000 - (number/10000000)*10); // Extract the 10^6 digit.
    numberHex += convertDecToASCIIHex(number/100000 - (number/1000000)*10); // Extract the 10^5 digit.
    numberHex += convertDecToASCIIHex(number/10000 - (number/100000)*10); // Extract the 10^4 digit.
    numberHex += convertDecToASCIIHex(number/1000 - (number/10000)*10); // Extract the 10^3 digit.
    numberHex += convertDecToASCIIHex(number/100 - (number/1000)*10); // Extract the 10^2 digit.
    numberHex += convertDecToASCIIHex(number/10 - (number/100)*10); // Extract the 10^1 digit.
    numberHex += convertDecToASCIIHex(number - (number/10)*10); // Extract the 10^0 digit.
  }

  else if(number >= 10000000){
    numberHex = convertDecToASCIIHex(number/10000000); // Extract the 10^7 digit.
    numberHex += convertDecToASCIIHex(number/1000000 - (number/10000000)*10); // Extract the 10^6 digit.
    numberHex += convertDecToASCIIHex(number/100000 - (number/1000000)*10); // Extract the 10^5 digit.
    numberHex += convertDecToASCIIHex(number/10000 - (number/100000)*10); // Extract the 10^4 digit.
    numberHex += convertDecToASCIIHex(number/1000 - (number/10000)*10); // Extract the 10^3 digit.
    numberHex += convertDecToASCIIHex(number/100 - (number/1000)*10); // Extract the 10^2 digit.
    numberHex += convertDecToASCIIHex(number/10 - (number/100)*10); // Extract the 10^1 digit.
    numberHex += convertDecToASCIIHex(number - (number/10)*10); // Extract the 10^0 digit.
  }

  else if(number >= 1000000){
    numberHex = convertDecToASCIIHex(number/1000000); // Extract the 10^6 digit.
    numberHex += convertDecToASCIIHex(number/100000 - (number/1000000)*10); // Extract the 10^5 digit.
    numberHex += convertDecToASCIIHex(number/10000 - (number/100000)*10); // Extract the 10^4 digit.
    numberHex += convertDecToASCIIHex(number/1000 - (number/10000)*10); // Extract the 10^3 digit.
    numberHex += convertDecToASCIIHex(number/100 - (number/1000)*10); // Extract the 10^2 digit.
    numberHex += convertDecToASCIIHex(number/10 - (number/100)*10); // Extract the 10^1 digit.
    numberHex += convertDecToASCIIHex(number - (number/10)*10); // Extract the 10^0 digit.
  }

  else if(number >= 100000){
    numberHex = convertDecToASCIIHex(number/100000); // Extract the 10^5 digit.
    numberHex += convertDecToASCIIHex(number/10000 - (number/100000)*10); // Extract the 10^4 digit.
    numberHex += convertDecToASCIIHex(number/1000 - (number/10000)*10); // Extract the 10^3 digit.
    numberHex += convertDecToASCIIHex(number/100 - (number/1000)*10); // Extract the 10^2 digit.
    numberHex += convertDecToASCIIHex(number/10 - (number/100)*10); // Extract the 10^1 digit.
    numberHex += convertDecToASCIIHex(number - (number/10)*10); // Extract the 10^0 digit.
  }

  else if(number >= 10000){
    numberHex = convertDecToASCIIHex(number/10000); // Extract the 10^4 digit.
    numberHex += convertDecToASCIIHex(number/1000 - (number/10000)*10); // Extract the 10^3 digit.
    numberHex += convertDecToASCIIHex(number/100 - (number/1000)*10); // Extract the 10^2 digit.
    numberHex += convertDecToASCIIHex(number/10 - (number/100)*10); // Extract the 10^1 digit.
    numberHex += convertDecToASCIIHex(number - (number/10)*10); // Extract the 10^0 digit.
  }
  
  else if(number >= 1000){
    numberHex = convertDecToASCIIHex(number/1000); // Extract the 10^3 digit.
    numberHex += convertDecToASCIIHex(number/100 - (number/1000)*10); // Extract the 10^2 digit.
    numberHex += convertDecToASCIIHex(number/10 - (number/100)*10); // Extract the 10^1 digit.
    numberHex += convertDecToASCIIHex(number - (number/10)*10); // Extract the 10^0 digit.
  }

  else if(number >= 100){
    numberHex = convertDecToASCIIHex(number/100); // Extract the 10^2 digit.
    numberHex += convertDecToASCIIHex(number/10 - (number/100)*10); // Extract the 10^1 digit.
    numberHex += convertDecToASCIIHex(number - (number/10)*10); // Extract the 10^0 digit.
  }

  else if(number >= 10){
    numberHex = convertDecToASCIIHex(number/10); // Extract the 10^1 digit.
    numberHex += convertDecToASCIIHex(number - (number/10)*10); // Extract the 10^0 digit.
  }

  else{
    numberHex = convertDecToASCIIHex(number); // Extract the 10^0 digit.
  }

  // --- Return the ASCII hex number. ---
  return numberHex;
  
} // End convertIntToASCIIHex().

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------- End data conversion and data processing functions. ---------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------







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


    // --- While contact with less than six satellites, and the user hasn't aborted. ---
    // --- Six satellites gives an accurate enough position (< 10 meter radius). ---
    while(sats < 6 && (!digitalRead(A0))){

      // --- Check how many connected sats, update sat number and sat icon on the display. ---
      sats = myGPS.getSIV();
      ssd1306_drawBitmap(82, 5, 11, 16, numbers[sats]);
      ssd1306_drawBitmap(96, 0, 32, 32, sat_icons[satIconNumber]);
      satIconNumber = (satIconNumber+1)%4;
      delay(1000);


    }

    // --- GPS now has contact with six satellites, or the user has aborted. ---
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










// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------- NB-IoT related functions ---------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ---------- Sets the RTC to match the time reported by the NB-IoT mobile network. ---------------------------------------
// ---------- Uses UTC+0. ----------
// ---------- Can also be used to check for internet connection since it doesn't... ----------
// ---------- ... set the time until a valid connection have been established. ----------
void getTheTime(void){

  // --- The command sent to the NB-IoT modem to ask for the time. ---
  String lookUp = "AT+CCLK?";

  // --- Declarations. ---
  String response = "";
  String responseStripped = "";
  int Year = 0;
  int Month = 0;
  int Date = 0;  
  int Hour = 0;
  int Minute = 0;
  int Second = 0;

  // --- Is false until the time have been set. ---
  bool gotTime = false;

  // --- While the time haven't been received. ---
  while(gotTime == false){

    // --- Ask the modem for the time. ---
    SerialUBLOX.println(lookUp);

    // --- Store the response from the modem. ---
    while (SerialUBLOX.available())
    {  
      char data = SerialUBLOX.read();
      response = response + data;
    }

    // --- This part of the code can be improved to be more readable. ---
    // --- Here the ASCII characters are used for comparison instead of the characters themselves. ---
    // --- If the fist part of the string reads '+CCLK', the time have been received, so extract the time/date. ---
    if(response[2] == 43 && response[3] == 67 && response[4] == 67 && response[5] == 76 && response[6] == 75 && response[7] == 58){
      Year = convertFromASCIIToBinary(response[10])*10 + convertFromASCIIToBinary(response[11]);
      Month = convertFromASCIIToBinary(response[13])*10 + convertFromASCIIToBinary(response[14]);
      Date = convertFromASCIIToBinary(response[16])*10 + convertFromASCIIToBinary(response[17]);
      Hour = convertFromASCIIToBinary(response[19])*10 + convertFromASCIIToBinary(response[20]);
      Minute = convertFromASCIIToBinary(response[22])*10 + convertFromASCIIToBinary(response[23]);
      Second = convertFromASCIIToBinary(response[25])*10 + convertFromASCIIToBinary(response[26]);

      // --- Set the RTC to match this time/date. ---
      rtc.setTime(Hour, Minute, Second);
      rtc.setDate(Date, Month, Year);

      // --- The time is now set. ---
      gotTime = true;
    }

    // --- Clear the response holder for before the next run (if getTime == false). ---
    response = "";
    responseStripped = "";

    // --- Delay 50 ms in order to not bombard the ublox module with requests. ---
    delay(50); 
  }
  
} // End getTheTime().





// ---------- Sets the RTC to match the time reported by the NB-IoT mobile network. ---------------------------------------
// ---------- Uses UTC+0. ----------
// ---------- Can also be used to check for internet connection since it doesn't... ----------
// ---------- ... set the time until a valid connection have been established. ----------
// ---------- Returns 0 if no connection is established within 130 seconds, otherwise retuns 1. ----------
int getTime130sTimeout(void){

  // --- The command sent to the NB-IoT modem to ask for the time. ---
  String lookUp = "AT+CCLK?";

  // --- Declarations. ---
  String response = "";
  String responseStripped = "";
  int Year = 0;
  int Month = 0;
  int Date = 0;  
  int Hour = 0;
  int Minute = 0;
  int Second = 0;

  // --- Is false until the time have been set. ---
  bool gotTime = false;

  // --- Store the start time, and declare a counter. ---
  uint32_t startTime = millis();
  uint32_t timeElapsed = 0;

  // --- While the time haven't been received and less than 130 ms have elapsed. ---
  while(gotTime == false && timeElapsed < 130000){

    // --- Ask the modem for the time. ---
    SerialUBLOX.println(lookUp);

    // --- Store the response from the modem. ---
    while (SerialUBLOX.available())
    {  
      char data = SerialUBLOX.read();
      response = response + data;
    }

    // --- This part of the code can be improved to be more readable. ---
    // --- Here the ASCII characters are used for comparison instead of the characters themselves. ---
    // --- If the fist part of the string reads '+CCLK', the time have been received, so extract the time/date. ---
    if(response[2] == 43 && response[3] == 67 && response[4] == 67 && response[5] == 76 && response[6] == 75 && response[7] == 58){
      Year = convertFromASCIIToBinary(response[10])*10 + convertFromASCIIToBinary(response[11]);
      Month = convertFromASCIIToBinary(response[13])*10 + convertFromASCIIToBinary(response[14]);
      Date = convertFromASCIIToBinary(response[16])*10 + convertFromASCIIToBinary(response[17]);
      Hour = convertFromASCIIToBinary(response[19])*10 + convertFromASCIIToBinary(response[20]);
      Minute = convertFromASCIIToBinary(response[22])*10 + convertFromASCIIToBinary(response[23]);
      Second = convertFromASCIIToBinary(response[25])*10 + convertFromASCIIToBinary(response[26]);

      // --- Set the RTC to match this time/date. ---
      rtc.setTime(Hour, Minute, Second);
      rtc.setDate(Date, Month, Year);

      // --- The time is now set, return 1. ---
      gotTime = true;
      return 1;
    }

    // --- Clear the response holder for before the next run (if getTime == false). ---
    response = "";
    responseStripped = "";

    // --- Check how much time have passed. ---
    timeElapsed = millis() - startTime;

    // --- Delay 50 ms in order to not bombard the ublox module with requests. ---
    delay(50);
  }

  // --- If this part of the code is reached, more than 130 seconds have passed. Return 0. ---
  return 0;
  
} // End of getTime130sTimeout().





// ---------- Returns 1 if the modem is connected to the netowrk. -----------------------------------------------------
// ---------- This function is outdated. It can be used as reference for later functions. ----------
// ---------- It can take some time after connection is made before the modem is ready to send. ----
// ---------- Use getTheTime() or getTime130sTimeout instead. --------------------------------------
int checkConnection(void){

  // --- The command to tell the modem to return the signal quality. And the empty response string. ---
  String check = "AT+CSQ";
  String response = "";

  // --- Tell the modem to return the signal quality. ---
  SerialUBLOX.println(check);

  // --- Give the modem some time to reply. ---
  delay(500);

  // --- Read the response from the modem. ---
  while (SerialUBLOX.available())
  {  
    response += SerialUBLOX.read();
  }

  // --- This part of the code can be improved to be more readable. ---
  // --- Here the ASCII characters are used for comparison instead of the characters themselves. ---
  // --- If this statement is correct, the correct response is received. ---
  if(response[6] == 54 && response[7] == 55 && response[8] == 56 && response[9] == 51 && response[10] == 56 && response[11] == 49){

    // --- If this statement is correct (character 17 is never 55 if a connection is made), return 1. ---
    if(response[17] != 55){
      return 1;
    }
    // --- Otherwise return 0 (no connection). ---
    else{return 0;}
  }

  // --- Return 0 if there aren't any respone. ---
  return 0;
  
} // End checkConnection().





// ---------- Sends data (must be in hex format) to the IP address and port number defined at the top of the main code. ---------------
void NBIoTSendData(String data){

  // --- Declarations. ---
  String openSocket = "AT+NSOCR=\"DGRAM\",17,42000,0";
  String prefix = "AT+NSOST=0,\"";
  String closeSocket = "AT+NSOCL=0";
  String dataLength = String(data.length()/2);

  // --- Build the string that must be sent to the modem. ---
  String fullSendString = prefix;
  fullSendString += SERVER_IP_ADDRESS;
  fullSendString += "\",";
  fullSendString += SERVER_PORT;
  fullSendString += ",";
  fullSendString += dataLength;
  fullSendString += ",\"";
  fullSendString += data;
  fullSendString += "\"";

  // --- Open socket, send data (twice), close socket. ---
  SerialUBLOX.println(openSocket);
  delay(500);
  SerialUBLOX.println(fullSendString);
  delay(1500);
  SerialUBLOX.println(fullSendString);
  delay(1500);
  SerialUBLOX.println(closeSocket);
  delay(500); 
  
} // End of NBIoTSendData().





// ---------- Send up to 10 uint32_t asset values to AllThingsTalk Maker. The parameter order must match the asset names declaired in the main code. ----------
// ---------- This function shuld have been split into more functions in order to shorten it even more. ----------
// ---------- See required JSON UDP syntax at https://docs.allthingstalk.com/developers/api/udp-messaging/. -----------
void sendToAllThingsTalk(uint32_t asset1Value, uint32_t asset2Value, uint32_t asset3Value, uint32_t asset4Value, uint32_t asset5Value, uint32_t asset6Value, uint32_t asset7Value, uint32_t asset8Value, uint32_t asset9Value, uint32_t asset10Value){
  
  String DeviceIDHex;
  String DeviceTokenHex;
  String Asset1Hex = "";
  String Asset2Hex = "";
  String Asset3Hex = "";
  String Asset4Hex = "";
  String Asset5Hex = "";
  String Asset6Hex = "";
  String Asset7Hex = "";
  String Asset8Hex = "";
  String Asset9Hex = "";
  String Asset10Hex = "";
  String Asset1ValueHex = "";
  String Asset2ValueHex = "";
  String Asset3ValueHex = "";
  String Asset4ValueHex = "";
  String Asset5ValueHex = "";
  String Asset6ValueHex = "";
  String Asset7ValueHex = "";
  String Asset8ValueHex = "";
  String Asset9ValueHex = "";
  String Asset10ValueHex = "";
  
  String BeforeFirstAssetHex = "0a7b22";                // = {"
  String AfterAssetHex = "223a7b2276616c7565223a";    // = ":{"value":
  String BeforeNewAssetHex = "7d2c22";                    // = },"
  String AfterLastValueHex = "7d7d";                  // = }}

  String openSocket = "AT+NSOCR=\"DGRAM\",17,42000,0";
  String Prefix = "AT+NSOST=0,\"40.68.172.187\",8891,";
  String messageLength = "";
  String message = "";
  String fullString = "";
  String closeSocket = "AT+NSOCL=0";
  
  // ----- Convert the prefix and asset names to hex strings. -----
  for(int i=0; i<strlen(DeviceID); i++){
    DeviceIDHex += String(DeviceID[i], HEX);
  }
  for(int i=0; i<strlen(DeviceToken); i++){
    DeviceTokenHex += String(DeviceToken[i], HEX);
  }
  for(int i=0; i<strlen(Asset1); i++){
    Asset1Hex += String(Asset1[i], HEX);
  }
  for(int i=0; i<strlen(Asset2); i++){
    Asset2Hex += String(Asset2[i], HEX);
  }
  for(int i=0; i<strlen(Asset3); i++){
    Asset3Hex += String(Asset3[i], HEX);
  }
  for(int i=0; i<strlen(Asset4); i++){
    Asset4Hex += String(Asset4[i], HEX);
  }
  for(int i=0; i<strlen(Asset5); i++){
    Asset5Hex += String(Asset5[i], HEX);
  }
  for(int i=0; i<strlen(Asset6); i++){
    Asset6Hex += String(Asset6[i], HEX);
  }
  for(int i=0; i<strlen(Asset7); i++){
    Asset7Hex += String(Asset7[i], HEX);
  }
  for(int i=0; i<strlen(Asset8); i++){
    Asset8Hex += String(Asset8[i], HEX);
  }
  for(int i=0; i<strlen(Asset9); i++){
    Asset9Hex += String(Asset9[i], HEX);
  }
  for(int i=0; i<strlen(Asset10); i++){
    Asset10Hex += String(Asset10[i], HEX);
  }

  // ----- Convert the asset values (function input) to a hex string. -----
  // ----- When a value turns negative, it jumps up to 0xFFFF. 0xFFFF is in other words equal to -1. -----
  // ----- If the values are >4100000000, they are interpreted as negative. "2D" is ASCII hex for "-". -----
  uint32_t maxPositiveValue = 4100000000;
  if(asset1Value > maxPositiveValue){
    asset1Value = 0xffffffff - asset1Value + 1; // Turn the values to their true value, but positive.
    Asset1ValueHex = "2D" + convertIntToASCIIHex(asset1Value); // Add "-" to make them negative.
  }
  else{
    Asset1ValueHex = convertIntToASCIIHex(asset1Value);
  }
  
  if(asset2Value > maxPositiveValue){
    asset2Value = 0xffffffff - asset2Value + 1;
    Asset2ValueHex = "2D" + convertIntToASCIIHex(asset2Value);
  }
  else{
    Asset2ValueHex = convertIntToASCIIHex(asset2Value);
  }
  
  if(asset3Value > maxPositiveValue){
    asset3Value = 0xffffffff - asset3Value + 1;
    Asset3ValueHex = "2D" + convertIntToASCIIHex(asset3Value);
  }
  else{
    Asset3ValueHex = convertIntToASCIIHex(asset3Value);
  }
  
  if(asset4Value > maxPositiveValue){
    asset4Value = 0xffffffff - asset4Value + 1;
    Asset4ValueHex = "2D" + convertIntToASCIIHex(asset4Value);
  }
  else{
    Asset4ValueHex = convertIntToASCIIHex(asset4Value);
  }
  
  if(asset5Value > maxPositiveValue){
    asset5Value = 0xffffffff - asset5Value + 1;
    Asset5ValueHex = "2D" + convertIntToASCIIHex(asset5Value);
  }
  else{
    Asset5ValueHex = convertIntToASCIIHex(asset5Value);
  }
  
  if(asset6Value > maxPositiveValue){
    asset6Value = 0xffffffff - asset6Value + 1;
    Asset6ValueHex = "2D" + convertIntToASCIIHex(asset6Value);
  }
  else{
    Asset6ValueHex = convertIntToASCIIHex(asset6Value);
  }

  if(asset7Value > maxPositiveValue){
    asset7Value = 0xffffffff - asset7Value + 1;
    Asset7ValueHex = "2D" + convertIntToASCIIHex(asset7Value);
  }
  else{
    Asset7ValueHex = convertIntToASCIIHex(asset7Value);
  }

  if(asset8Value > maxPositiveValue){
    asset8Value = 0xffffffff - asset8Value + 1;
    Asset8ValueHex = "2D" + convertIntToASCIIHex(asset8Value);
  }
  else{
    Asset8ValueHex = convertIntToASCIIHex(asset8Value);
  }

  if(asset9Value > maxPositiveValue){
    asset9Value = 0xffffffff - asset9Value + 1;
    Asset9ValueHex = "2D" + convertIntToASCIIHex(asset9Value);
  }
  else{
    Asset9ValueHex = convertIntToASCIIHex(asset9Value);
  }

  if(asset10Value > maxPositiveValue){
    asset10Value = 0xffffffff - asset10Value + 1;
    Asset10ValueHex = "2D" + convertIntToASCIIHex(asset10Value);
  }
  else{
    Asset10ValueHex = convertIntToASCIIHex(asset10Value);
  }

  // ----- Build the ASCII hex message. -----
  message = DeviceIDHex + "0a" + DeviceTokenHex + "0a7b22" + Asset1Hex + AfterAssetHex + Asset1ValueHex;

  if(Asset2 != ""){
    message += BeforeNewAssetHex + Asset2Hex + AfterAssetHex + Asset2ValueHex;
  }

  if(Asset3 != ""){
    message += BeforeNewAssetHex + Asset3Hex + AfterAssetHex + Asset3ValueHex;
  }

  if(Asset4 != ""){
    message += BeforeNewAssetHex + Asset4Hex + AfterAssetHex + Asset4ValueHex;
  }

  if(Asset5 != ""){
    message += BeforeNewAssetHex + Asset5Hex + AfterAssetHex + Asset5ValueHex;
  }

  if(Asset6 != ""){
    message += BeforeNewAssetHex + Asset6Hex + AfterAssetHex + Asset6ValueHex;
  }

  if(Asset7 != ""){
    message += BeforeNewAssetHex + Asset7Hex + AfterAssetHex + Asset7ValueHex;
  }

  if(Asset8 != ""){
    message += BeforeNewAssetHex + Asset8Hex + AfterAssetHex + Asset8ValueHex;
  }

  if(Asset9 != ""){
    message += BeforeNewAssetHex + Asset9Hex + AfterAssetHex + Asset9ValueHex;
  }

  if(Asset10 != ""){
    message += BeforeNewAssetHex + Asset10Hex + AfterAssetHex + Asset10ValueHex;
  }

  message += AfterLastValueHex;

  messageLength = String(message.length()/2); // Divides by two because one hex contains two ASCII characters.
  
  fullString = Prefix + messageLength + ",\"" + message + "\"";

  // --- Sends the message twice to be sure since it's UDP. ---
  SerialUBLOX.println(openSocket);
  delay(500);
  SerialUBLOX.println(fullString);
  delay(1500);
  SerialUBLOX.println(fullString);
  delay(1500);
  SerialUBLOX.println(closeSocket);
  delay(500); 

} // End sendToAllThingsTalk().





// ---------- Send data that have not yet been sent due to connection timeout. ----------
void sendUnsentData(){

  // --- Go through the array created by storeUnsentData(). ---
  for(int i=0; i<100; i++){

    // --- If the unsent_Status is 1, the message have not been sent. ---
    if(unsent_Status[i] == 1){

      // --- Send this unsent data, and clear the unsent_Status of that package. ---
      sendToAllThingsTalk(unsent_temperature[i], unsent_connectionTime[i], unsent_depth[i], unsent_batteryVoltage[i], unsent_counter[i]);
      unsent_Status[i] = 0;
    }
  }
  
} // End sendUnsentData().





// ---------- Store data that has not been sent in RAM. ----------
// ---------- Can store up to 100 sets of data. ----------
// ---------- This function shuld be updated to instead take the data as input. ----------
void storeUnsentData(){

  // --- Loop through the pre-declared list. ---
  for(int i=0; i<100; i++){

    // --- If the unsent_Status is 0, the array position is free. ---
    if(unsent_Status[i] == 0){

      // --- Store data in this position and mark as occupied. ---
      // --- unsent_Status is cleard when the data gets sent. ---
      unsent_Status[i] = 1;
      unsent_year[i] = yearNow;
      unsent_month[i] = monthNow;
      unsent_date[i] = dateNow;
      unsent_hour[i] = hoursNow;
      unsent_minute[i] = minutesNow;
      unsent_connectionTime[i] = connectionTime;
      unsent_depth[i] = depth;
      unsent_batteryVoltage[i] = batteryVoltage;
      unsent_temperature[i] = temperature;
      unsent_counter[i] = counter;

      // --- No need to loop through the rest of the list once the data is stored. ---
      return;
    }
  }
  
} // End storeUnsentData().

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------- End NB-IoT related functions ---------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------







// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------- User input functions ----------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ---------- The user uses the encoder to unput a 4-digit number. The numbers shows on the display on the "calibration-screen". -----------
// ---------- The inputCurrentDepthMASL() function is used no to input a 7-digit number, but this function might be needed later. -----
uint16_t inputCurrentDepth(void){

    // --- Initiate the display. ---
    ssd1306_128x64_i2c_init();
    ssd1306_fillScreen( 0x00 );

    // --- Show the "calibration-screen". Start calibrating the highest digit (the 10^3 position). ---
    ssd1306_drawBitmap(0, 0, 128, 64, depth_calibration);
    ssd1306_drawBitmap(67, 5, 11, 16, numbers[0]);
    ssd1306_drawBitmap(77, 5, 11, 16, numbers[0]);
    ssd1306_drawBitmap(87, 5, 11, 16, numbers[0]);
    ssd1306_drawBitmap(97, 5, 11, 16, numbers[0]);
    ssd1306_drawBitmap(68, 7, 38, 8, dash[3]);

    // --- Declare variables. ---
    uint32_t depthNow = 0;
    int CLKVal = 0;
    int CLKValLast = 0;
    int i[4] = {0};
    int ii = 3;
    int dash_i = 3;
    int isCalibrating = true;

    // --- Read current pin state of the encoders CLK pin. ---
    // --- See http://henrysbench.capnfatz.com/henrys-bench/arduino-sensors-and-input/keyes-ky-040-arduino-rotary-encoder-user-manual/...
    // --- ... for encoders internal construction and code example for encoding the rotation. ---
    CLKValLast = digitalRead(ROTATORY_CLK_PIN);

    // --- While the user is still calibrating. ---
    while(isCalibrating)
    {     
      
      // --- Some delays are added certain places to avoid debouncing. ---
      delay(1);

      // --- See URL above for how to interpret the rotation. ---
      CLKVal = digitalRead(ROTATORY_CLK_PIN);
      if(CLKVal != CLKValLast){
        if(!CLKVal){
          delay(1);

          // --- If rotation to the left, and the current digit is higher than zero, subtract one from the number. ---
          // --- Update the display with the new number. ---
          // --- The "dash" is a line under the digit being edited. ---
          if(digitalRead(ROTATORY_DT_PIN) != CLKVal){
            if(i[ii] != 0){
              i[ii]--;
              ssd1306_drawBitmap(67, 5, 11, 16, numbers[i[3]]);
              ssd1306_drawBitmap(77, 5, 11, 16, numbers[i[2]]);
              ssd1306_drawBitmap(87, 5, 11, 16, numbers[i[1]]);
              ssd1306_drawBitmap(97, 5, 11, 16, numbers[i[0]]);
              ssd1306_drawBitmap(68, 7, 38, 8, dash[dash_i]);
              delay(5);  
            }
          }
          // --- Else it's rotated to the rate. Add one unless the number already is nine. ---
          else{
            if(i[ii] != 9){
              i[ii]++;
              ssd1306_drawBitmap(67, 5, 11, 16, numbers[i[3]]);
              ssd1306_drawBitmap(77, 5, 11, 16, numbers[i[2]]);
              ssd1306_drawBitmap(87, 5, 11, 16, numbers[i[1]]);
              ssd1306_drawBitmap(97, 5, 11, 16, numbers[i[0]]);
              ssd1306_drawBitmap(68, 7, 38, 8, dash[dash_i]);
              delay(5);
            }
          
          } 
        }
      }

      // --- Read the current CLK pin calue to further interpret the encoding. ---
      CLKValLast = CLKVal;

      // --- If the pushputton on the encoder is pressed. ---
      if(digitalRead(ROTATORY_SW_PIN)){
        // --- Wait until the pushbutton released again. ---
        while(digitalRead(ROTATORY_SW_PIN)){}

        // --- If the current position is 10^0, the calibration is done. ---
        // --- Calculate the user input number (depthNow) from the inputs. ---
        if(ii == 0){
          isCalibrating = false;
          depthNow = (uint16_t)(i[3]*1000) + (uint16_t)(i[2]*100) + i[1]*10 + i[0];
        }
        // --- If the current digit isn't the 10^0 position, the calibration is not finished. ---
        // --- Move the dash and change the number being calibrated. --- 
        else{
          ii--;
          dash_i--;
          ssd1306_drawBitmap(68, 7, 38, 8, dash[dash_i]);
          delay(200);
        }
      }
    } // End while(isCalibrating).

    // --- Return the user input number. ---
    return depthNow;
    
} // End inputCurrentDepth().


// ---------- The user uses the encoder to unput a 7-digit number. The numbers shows on the display on the "calibration-screen". -----------
// ---------- Current depth in meters above sea level. 1 millimetre resolution. -------------------------------------------------
// ---------- Change name to inputCurrentWaterHeightMASL? -----------------------------------------------------------
uint32_t inputCurrentDepthMASL(void){

    // --- Initiate the display. ---
    ssd1306_128x64_i2c_init();
    ssd1306_fillScreen( 0x00 );

    // --- Show the "calibration-screen". Start calibrating the highest digit (10^3). ---
    ssd1306_drawBitmap(0, 0, 128, 64, depth_calibrationMASL);
    ssd1306_drawBitmap(16, 5, 11, 16, numbers[0]);
    ssd1306_drawBitmap(26, 5, 11, 16, numbers[0]);
    ssd1306_drawBitmap(36, 5, 11, 16, numbers[0]);
    ssd1306_drawBitmap(46, 5, 11, 16, numbers[0]);
    ssd1306_drawBitmap(61, 5, 11, 16, numbers[0]);
    ssd1306_drawBitmap(71, 5, 11, 16, numbers[0]);
    ssd1306_drawBitmap(81, 5, 11, 16, numbers[0]);
    ssd1306_drawBitmap(17, 7, 73, 8, dash_depthMASL[6]);

    // --- Declare variables. ---
    uint32_t depthNow = 0;
    int CLKVal = 0;
    int CLKValLast = 0;
    int i[7] = {0};
    int ii = 6;
    int dash_i = 6;
    int isCalibrating = true;

    // --- Read current pin state of the encoders CLK pin. ---
    // --- See http://henrysbench.capnfatz.com/henrys-bench/arduino-sensors-and-input/keyes-ky-040-arduino-rotary-encoder-user-manual/...
    // --- ... for encoders internal construction and code example for encoding the rotation. ---
    CLKValLast = digitalRead(ROTATORY_CLK_PIN);

    // --- While the user is still calibrating. ---
    while(isCalibrating)
    {     
      
      // --- Some delays are added certain places to avoid debouncing. ---
      delay(1);

      // --- See URL above for how to interpret the rotation. ---
      CLKVal = digitalRead(ROTATORY_CLK_PIN);
      if(CLKVal != CLKValLast){
        if(!CLKVal){
          delay(1);

          // --- If rotation to the left, and the current digit is higher than zero, subtract one from the number. ---
          // --- Update the display with the new number. ---
          // --- The "dash" is a line under the digit being edited. ---
          if(digitalRead(ROTATORY_DT_PIN) != CLKVal){
            if(i[ii] != 0){
              i[ii]--;
              ssd1306_drawBitmap(16, 5, 11, 16, numbers[i[6]]);
              ssd1306_drawBitmap(26, 5, 11, 16, numbers[i[5]]);
              ssd1306_drawBitmap(36, 5, 11, 16, numbers[i[4]]);
              ssd1306_drawBitmap(46, 5, 11, 16, numbers[i[3]]);
              ssd1306_drawBitmap(61, 5, 11, 16, numbers[i[2]]);
              ssd1306_drawBitmap(71, 5, 11, 16, numbers[i[1]]);
              ssd1306_drawBitmap(81, 5, 11, 16, numbers[i[0]]);
              ssd1306_drawBitmap(17, 7, 73, 8, dash_depthMASL[dash_i]);
              delay(5);  
            }
          }
          // --- Else it's rotated to the rate. Add one unless the number already is nine. ---
          else{
            if(i[ii] != 9){
              i[ii]++;
              ssd1306_drawBitmap(16, 5, 11, 16, numbers[i[6]]);
              ssd1306_drawBitmap(26, 5, 11, 16, numbers[i[5]]);
              ssd1306_drawBitmap(36, 5, 11, 16, numbers[i[4]]);
              ssd1306_drawBitmap(46, 5, 11, 16, numbers[i[3]]);
              ssd1306_drawBitmap(61, 5, 11, 16, numbers[i[2]]);
              ssd1306_drawBitmap(71, 5, 11, 16, numbers[i[1]]);
              ssd1306_drawBitmap(81, 5, 11, 16, numbers[i[0]]);
              ssd1306_drawBitmap(17, 7, 73, 8, dash_depthMASL[dash_i]);
              delay(5);
            }
          
          } 
        }
      }

      // --- Read the current CLK pin calue to further interpret the encoding. ---
      CLKValLast = CLKVal;

       // --- If the pushputton on the encoder is pressed. ---
      if(digitalRead(ROTATORY_SW_PIN)){
        // --- Wait until the pushbutton released again. ---
        while(digitalRead(ROTATORY_SW_PIN)){}

        // --- If the current position is 10^0, the calibration is done. ---
        // --- Calculate the user input number (depthNow) from the inputs. ---
        if(ii == 0){
          isCalibrating = false;
          depthNow = (uint32_t)(i[6]*1000000) + (uint32_t)(i[5]*100000) + (uint32_t)(i[4]*10000) + (uint16_t)(i[3]*1000) + (uint16_t)(i[2]*100) + i[1]*10 + i[0];
        }
        // --- If the current digit isn't the 10^0 position, the calibration is not finished. ---
        // --- Move the dash and change the number being calibrated. --- 
        else{
          ii--;
          dash_i--;
          ssd1306_drawBitmap(17, 7, 73, 8, dash_depthMASL[dash_i]);
          delay(200);
        }
      }
    } // End while(isCalibrating).

    // --- Return the user input number. ---
    return depthNow;

} // End inputCurrentDepthMASL().





// ---------- The user inputs a 4-digit number. This number is returned by the function. -------------------------------------
// ---------- This function works just like the inputCurrentDepth() function, excepth that a "password screen" is shown. -----
uint16_t inputPassword(void){

    // --- Initiate the display. ---
    ssd1306_128x64_i2c_init();
    ssd1306_fillScreen( 0x00 );

    // --- Show the "calibration-screen". Start calibrating the highest digit (10^3). ---
    ssd1306_drawBitmap(0, 0, 128, 64, passwordInput);
    ssd1306_drawBitmap(52, 5, 11, 16, numbers[0]);
    ssd1306_drawBitmap(62, 5, 11, 16, numbers[0]);
    ssd1306_drawBitmap(72, 5, 11, 16, numbers[0]);
    ssd1306_drawBitmap(82, 5, 11, 16, numbers[0]);
    ssd1306_drawBitmap(53, 7, 38, 8, dash[3]);

    // --- Declare variables. ---
    uint16_t passwordInput = 0;
    int CLKVal = 0;
    int CLKValLast = 0;
    int i[4] = {0};
    int ii = 3;
    int dash_i = 3;
    int notFinished = true;

    // --- Read current pin state of the encoders CLK pin. ---
    // --- See http://henrysbench.capnfatz.com/henrys-bench/arduino-sensors-and-input/keyes-ky-040-arduino-rotary-encoder-user-manual/...
    // --- ... for encoders internal construction and code example for encoding the rotation. ---
    CLKValLast = digitalRead(ROTATORY_CLK_PIN);

    // --- While the user is still inputting the password. ---
    while(notFinished)
    {     

      // --- Some delays are added certain places to avoid debouncing. ---
      delay(1);

      // --- See URL above for how to interpret the rotation. ---
      CLKVal = digitalRead(ROTATORY_CLK_PIN);
      if(CLKVal != CLKValLast){
        if(!CLKVal){
          delay(1);

          // --- If rotation to the left, and the current digit is higher than zero, subtract one from the number. ---
          // --- Update the display with the new number. ---
          // --- The "dash" is a line under the digit being edited. ---
          if(digitalRead(ROTATORY_DT_PIN) != CLKVal){
            if(i[ii] != 0){
              i[ii]--;
              ssd1306_drawBitmap(52, 5, 11, 16, numbers[i[3]]);
              ssd1306_drawBitmap(62, 5, 11, 16, numbers[i[2]]);
              ssd1306_drawBitmap(72, 5, 11, 16, numbers[i[1]]);
              ssd1306_drawBitmap(82, 5, 11, 16, numbers[i[0]]);
              ssd1306_drawBitmap(53, 7, 38, 8, dash[dash_i]);
              delay(5);  
            }
          }
          // --- Else it's rotated to the rate. Add one unless the number already is nine. ---
          else{
            if(i[ii] != 9){
              i[ii]++;
              ssd1306_drawBitmap(52, 5, 11, 16, numbers[i[3]]);
              ssd1306_drawBitmap(62, 5, 11, 16, numbers[i[2]]);
              ssd1306_drawBitmap(72, 5, 11, 16, numbers[i[1]]);
              ssd1306_drawBitmap(82, 5, 11, 16, numbers[i[0]]);
              ssd1306_drawBitmap(53, 7, 38, 8, dash[dash_i]);
              delay(5);
            }
          
          } 
        }
      }

      // --- Read the current CLK pin calue to further interpret the encoding. ---
      CLKValLast = CLKVal;

      // --- If the pushputton on the encoder is pressed. ---
      if(digitalRead(ROTATORY_SW_PIN)){
        // --- Wait until the pushbutton released again. ---
        while(digitalRead(ROTATORY_SW_PIN)){}

        // --- If the current position is 10^0, the password input is done. ---
        // --- Calculate the user input number (passwordInput) from the inputs. ---
        if(ii == 0){
          notFinished = false;
          passwordInput = (uint16_t)(i[3]*1000) + (uint16_t)(i[2]*100) + i[1]*10 + i[0];
        }
        // --- If the current digit isn't the 10^0 position, the calibration is not finished. ---
        // --- Move the dash and change the number being calibrated. --- 
        else{
          ii--;
          dash_i--;
          ssd1306_drawBitmap(53, 7, 38, 8, dash[dash_i]);
          delay(200);
        }
      }
    } // End while(notFinished).

    // --- Return the user input number. ---
    return passwordInput;
    
} // End inputPassword().


// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------- End user input functions ---------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------






// ---------- Writes 8-bits of data via I2C to a register at an address. --------------------------------
void I2CWrite8bits(uint8_t I2CAddress, uint8_t reg, uint8_t val){

   // --- Establish communication with a device on a given address. ---
   Wire.beginTransmission((uint8_t)I2CAddress);

   // --- Tell the device wich register you want to write to. ---
   Wire.write((uint8_t)reg);

   // --- Write the data to the chosen register. ---
   Wire.write((uint8_t)val);

   // --- End the communication. Release the I2C bus. ---
   Wire.endTransmission();
   
} // Enda I2CWrite8bits().





// ---------- Read 16 bits of data via I2C from a register at an address. ------------------------------------
uint16_t I2CRead16bits(uint8_t I2CAddress, uint8_t reg){

    // --- Placeholder for the value to be returned. ---
    uint16_t value;

    // --- Establish communication with a device on a given address. ---
    Wire.beginTransmission((uint8_t)I2CAddress);

    // --- Tell the device wich register you want to read from. ---
    Wire.write((uint8_t)reg);

    // --- End the communication. Release the I2C bus. ---
    Wire.endTransmission();

    // --- Request the data from the previously chosen register at the given address. ---
    Wire.requestFrom((uint8_t)I2CAddress, (byte)2);

    // --- Read one byte of data, bit shift 8 places before read the next byte to make one 16-bit number. ---
    value = (Wire.read() << 8) | Wire.read();

    // --- Return the 16-bit number. ---
    return value;
}






uint16_t I2CRead16bits_LE(uint8_t I2CAddress, uint8_t reg) {
  uint16_t temp = I2CRead16bits(I2CAddress, reg);
  return (temp >> 8) | (temp << 8);
}





// ---------- Read 24 bits of date via I2C from a register at an address. ------------------------------------
uint32_t I2CRead24bits(uint8_t I2CAddress, uint8_t reg){

    // --- Placeholder for the value to be returned. ---
    uint32_t value;

    // --- Establish communication with a device on a given address. ---
    Wire.beginTransmission((uint8_t)I2CAddress);

    // --- Tell the device wich register you want to read from. ---
    Wire.write((uint8_t)reg);

    // --- End the communication. Release the I2C bus. ---
    Wire.endTransmission();

    // --- Request the data from the previously chosen register at the given address. ---
    Wire.requestFrom((uint8_t)I2CAddress, (byte)3);

    // --- Read byte and bit shift 8 bits to give "room" for the next byte. ---
    value = Wire.read();
    value <<= 8;
    value |= Wire.read();
    value <<= 8;
    value |= Wire.read();

    // --- Return the 24-bit number (as uint32_t). ---
    return value;

} // End I2CRead24Bits().






// ---------- Writes 16-bits to the non-volatile flash memory (will be erased upon re-programming). -----------------------------------------------------
// ---------- Uses flash memory as simulated EEPROM since ther aren't ane EEPROM on the SAMD21. --------------------------------------------------------
// ---------- The FlashAsEEPROM library only handles 8-bits at a time. This function handles 16-bits at one. ------
// ---------- The start address is the address of the most significant byte. -----------------------
// ---------- NOTE: EEPROM.commit(); must be called after this (and the rest of the EEPROM function) to actually store the data. ----
void EEPROMUpdate16bits(uint8_t startAddress, uint16_t data){

  // --- Separate the 16-bit number into two 8-bit numbers. ---
  uint8_t MSB = ((data>>8) & 0xff);
  uint8_t LSB = (data & 0xff);

  // --- Update EEPROM with both bytes.---
  // --- Update means that no data is written if the new and old data at the address is the same. ---
  EEPROM.update(startAddress, MSB);
  EEPROM.update(startAddress+1, LSB);
  
} // End EEPROMUpdate16bits().





// ---------- Reads 16-bits from the non-volatile flash memory (will be erased upon re-programming). -----------------------------------------------------
// ---------- Uses flash memory as simulated EEPROM since ther aren't ane EEPROM on the SAMD21. --------------------------------------------------------
// ---------- The FlashAsEEPROM library only handles 8-bits at a time. This function handles 16-bits at one. ------
// ---------- The start address is the address of the most significant byte. -----------------------
uint16_t EEPROMRead16bits(uint8_t startAddress){

  // --- Read two 8-bit numbers from the given flash addresses. ---
  uint8_t MSB = EEPROM.read(startAddress);
  uint8_t LSB = EEPROM.read(startAddress+1);

  // --- Combine the two 8-bit numbers to one 16-bit number. ---
  uint16_t fullData = (((MSB<<8) & 0xffff) + (LSB & 0xffff));

  // --- Return the 16-bit number. ---
  return fullData;
  
} // End EEPROMRead16bits().


void EEPROMUpdate32bits(uint8_t startAddress, uint32_t data){
  uint8_t high = ((data>>24) & 0xff);
  uint8_t midHigh = ((data>>16) & 0xff);
  uint8_t midLow = ((data>>8) & 0xff);
  uint8_t low = (data & 0xff);

  EEPROM.update(startAddress, high);
  EEPROM.update(startAddress+1, midHigh);
  EEPROM.update(startAddress+2, midLow);
  EEPROM.update(startAddress+3, low);
}





// ---------- Writes 32-bits to the non-volatile flash memory (will be erased upon re-programming). -----------------------------------------------------
// ---------- Uses flash memory as simulated EEPROM since ther aren't ane EEPROM on the SAMD21. --------------------------------------------------------
// ---------- The FlashAsEEPROM library only handles 8-bits at a time. This function handles 32-bits at one. ------
// ---------- The start address is the address of the most significant byte. -----------------------
// ---------- NOTE: EEPROM.commit(); must be called after this (and the rest of the EEPROM function) to actually store the data. ----
uint32_t EEPROMRead32bits(uint8_t startAddress){

  // --- Read four 8-bit numbers from the given flash addresses. ---
  uint8_t high = EEPROM.read(startAddress);
  uint8_t midHigh = EEPROM.read(startAddress+1);
  uint8_t midLow = EEPROM.read(startAddress+2);
  uint8_t low = EEPROM.read(startAddress+3);

  // --- Combine the four 8-bit numbers to one 32-bit number. ---
  uint32_t fullData = (((high<<24) & 0xffffffff) + ((midHigh<<16) & 0xffffffff) + ((midLow<<8) & 0xffffffff) + (low & 0xffffffff));

  // --- Return the 32-bit number. ---
  return fullData;
  
} // End EEPROMRead32bits().



// ---------- Waits for the modem to connect to the network, then shows the signal strength on the display. -------------------------
// ---------- Can show the signal strength as dBm or a linearly mapped percentage. ---------------------------------------------
void showSignalStrength(void){

  // --- The command that tells the modem to return the signal strenght. ---
  String lookUp = "AT+CSQ";

  // --- Declarations. ---
  String response = "";
  String responseStripped = "";
  uint8_t strength = 0;

  // --- Is true when the modem is connected and a valid connection strenght is returned. ---
  bool gotStrength = false;

  // --- While there is no valid signal strenght. ---
  while(gotStrength == false){

    // --- Ask modem for signal strenght. ---
    SerialUBLOX.println(lookUp);

    // --- Read the reply from the modem. ---
    while (SerialUBLOX.available())
    {  
      char data = SerialUBLOX.read();
      response += data;
    }

    // --- If a valid response is returned. ---
    if(response[3] == 'C' && response[4] == 'S' && response[5] == 'Q'){
      // --- If the 9th character is a ',', the strength is a one-digit number. ---
      if(response[9] == ','){
        strength = convertFromASCIIToBinary(response[8]);
      }
      // --- If not, it's a two digit number. ---
      else{
        strength = convertFromASCIIToBinary(response[8])*10 + convertFromASCIIToBinary(response[9]);
      }

      // --- No need to keep searching. ---
      gotStrength = true;

      // --- Decide which value to map to. RSSI or percent. Comment out one of the two lines. ---
      //strength = map(strength, 0, 31, 113, 51); // Map from "signal_power" to RSSI.
      strength = map(strength, 0, 31, 0, 100); // Map from "signal_power" to percent.

      // --- Convert to a three digit number. ---
      uint8_t strength3 = strength/100;
      uint8_t strength2 = strength/10 - (strength/100)*10;
      uint8_t strength1 = strength - (strength/10)*10;

      // Decide which value to display on the screen. Comment out one of the two lines. ---
      //ssd1306_drawBitmap(0, 0, 128, 64, signal_strength_dBm);
      ssd1306_drawBitmap(0, 0, 128, 64, signal_strength_percent);

      // --- Display the numbers on the screen. ---
      ssd1306_drawBitmap(58, 5, 11, 16, numbers[strength3]);
      ssd1306_drawBitmap(68, 5, 11, 16, numbers[strength2]);
      ssd1306_drawBitmap(78, 5, 11, 16, numbers[strength1]);
    }

    // --- Clrear the response placeholder. ---
    response = "";
    responseStripped = "";

    // --- Delay 50 ms in order to not bombard the ublox module with requests. ---
    delay(50);
    
  } // End while(gotStrength == false).
  
} // End showSignalStrength().
