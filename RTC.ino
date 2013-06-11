// TODO doc this and don't use String so that we can save memory

void setupRTC() {
  Wire.begin();
  RTC.begin();
  
  for (int x=0; x<5; x++) {
    if (RTC.isrunning()) {
      rtcAvail = true;
      break;
    } else {
      Serial.println("Waiting for RTC...");
      delay(500);
    }
  }
}
       
void setDateTime() {
  if (!rtcAvail) {
    Serial.println("RTC not available, datetime not set.");
  } else {
    RTC.adjust(DateTime((uint16_t) 2013, (uint8_t) 05, (uint8_t) 21, (uint8_t) 15, (uint8_t) 24, (uint8_t) 30));
    Serial.println("Setting RTC to hardcoded time.");
  }
}

String getDateStr(DateTime dateTime) {
  
  char buffer32[32];
  
  String theYear  = "2001";
  String theMonth = "1";
  String theDay   = "1"; 
  
  if (rtcAvail) {
    theYear  = itoa(dateTime.year(), buffer32, 10);
    theMonth = itoa(dateTime.month(), buffer32, 10);
    theDay   = itoa(dateTime.day(), buffer32, 10);
  }
  
  if (theMonth.length() < 2) theMonth = "0" + theMonth;  
  if (theDay.length()   < 2) theDay   = "0" + theDay;  
  
  return theYear + "/" + theMonth + "/" + theDay;
}

String getTimeStr(DateTime dateTime) {

  char buffer32[32];
  
  String theHour   = "5";
  String theMinute = "4";
  String theSecond = "3";

  if (rtcAvail) {
    theHour   = itoa(dateTime.hour(), buffer32, 10);
    theMinute = itoa(dateTime.minute(), buffer32, 10);
    theSecond = itoa(dateTime.second(), buffer32, 10);
  }
  
  if (theHour.length()   < 2) theHour   = "0" + theHour;
  if (theMinute.length() < 2) theMinute = "0" + theMinute;  
  if (theSecond.length() < 2) theSecond = "0" + theSecond;
  
  return theHour + ":" +  theMinute  + ":" + theSecond;
}

void printDateTime(DateTime dateTime) {
    Serial.print(dateTime.year(), DEC);
    Serial.print('/');
    Serial.print(dateTime.month(), DEC);
    Serial.print('/');
    Serial.print(dateTime.day(), DEC);
    Serial.print(' ');
    Serial.print(dateTime.hour(), DEC);
    Serial.print(':');
    Serial.print(dateTime.minute(), DEC);
    Serial.print(':');
    Serial.print(dateTime.second(), DEC);
    Serial.println();
}





