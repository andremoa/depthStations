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
