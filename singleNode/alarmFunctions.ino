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





// ---------- The function that is set to run once when the interrupt is triggered. -----
void alarmMatch(){
  rtc.disableAlarm(); // Disable the alarm.
}





// ---------- Puts the microcontroller into deep sleep (standby) mode. ------------
void goToSleep(){
  
  // --- Choose standby mode. ---
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

  // --- Go to sleep. ---
  __WFI();
  
} // End goToSleep().


// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------- End alarm functions -----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
