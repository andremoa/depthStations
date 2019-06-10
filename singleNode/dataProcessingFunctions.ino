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



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------- End data conversion and data processing functions. ---------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
