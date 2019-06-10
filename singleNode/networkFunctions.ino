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




// ---------- Configure the modem to work with the Telia network. -------------------------
// ---------- The backslash allows for quotation marks to be usen inside a string. --------
void configureModemSettingsTelia(){
  SerialUBLOX.println("AT+NCONFIG=\"AUTOCONNECT\",\"TRUE\"");
  delay(100);
  SerialUBLOX.println("AT+NCONFIG=\"CR_0354_0338_SCRAMBLING\",\"TRUE\"");
  delay(100);
  SerialUBLOX.println("AT+NCONFIG=\"CR_0859_SI_AVOID\",\"TRUE\"");
  delay(100);
  SerialUBLOX.println("AT+NCONFIG=\"COMBINE_ATTACH\",\"FALSE\"");
  delay(100);
  SerialUBLOX.println("AT+NCONFIG=\"CELL_RESELECTION\",\"FALSE\"");
  delay(100);
  SerialUBLOX.println("AT+NCONFIG=\"ENABLE_BIP\",\"FALSE\"");
  delay(100);
  SerialUBLOX.println("AT+NCONFIG=\"NAS_SIM_POWER_SAVING_ENABLE\",\"TRUE\"");
  
} // End configureModemSettingsTelia().





// ---------- Send data that have not yet been sent due to connection timeout. ----------
// ---------- This function shuld be updated once the Telenor server is ready. ----------
// ---------- Hard to write and test a function before it can be used. ---------
// ---------- The new function only needs two arrays, one for status and one for the JSON data string. --------
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
// ---------- Take instead the JSON string as argument, and store it toghether with the unsent_Status. ------
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

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------- End NB-IoT related functions ---------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
