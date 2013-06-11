
/**
 * Utility functions for 1602 and 2004 type LCD screens
 * that use the LiquidCrystal_I2C library.
 *
 * Requires:
 *    LiquidCrystal_I2C library to e included
 *    lcdBuffer to be defined as an String array, used to
 *      hold what is currently being displayed on lcd screen
 *    lcdRows to be number of rows lcd can display 
 *    lcdCols to be number of columns lcd can display
 */

void setupLcd()
{
  lcd.begin(lcdRows, lcdCols);  
  setBacklight(false); // switch on the backlight
  //setLcdBacklightTimer(); // turn off in 2 minutes
  lcd.clear();
  
  initLcdBuffer();
}

void setBacklight(boolean state) {
  if (state == true) {
    lcd.setBacklight(1);
  } else {
    lcd.setBacklight(0);
  }
}

void initLcdBuffer() {
  for (int i=0; i<lcdRows; i++) { 
    lcdBuffer[i] = ""; 
  }  

}

void setLcdBufferRow(int row, String columns) {
  // TODO add check of row bounds
  // TODO trim at column bounds
  lcdBuffer[row] = columns; 
}

void refreshLcd() {
  for (int i=0; i<lcdRows; i++) { 
    lcd.setCursor(0,i);  // column 0 on row i
    lcd.print(lcdBuffer[i]);
  } 
}

void blankLcdBuffer() {
  String blankLine = "";
  for (int i=0; i<lcdCols; i++) {
    blankLine += "0"; 
  }
  
  for (int i=0; i<lcdRows; i++) { 
    lcdBuffer[i] = blankLine; 
  }  
}

/* Serialize contents of lcdLines, useful for debugging */
String getLcdBufferStr() {
  String lcdBufferStr = "";   
  for (int i=0; i<lcdRows; i++) { 
    lcdBufferStr += lcdBuffer[i] + "\n";
  }
  return lcdBufferStr;
}

void dumpLcdBuffer() {
  for (int i=0; i<lcdRows; i++) { 
    Serial.println(lcdBuffer[i]);
  }
}


void handleLcdBacklightButton() {
  // switch is not hw debounced, capture switch contacts here.
  numLcdBacklightButtonPresses++;
}

void checkLcdBacklightButton() {
  
  if (numLcdBacklightButtonPresses > 0) {
    // button was pressed since we last checked
   
    // sleep a bit to capture rest of bounces
    delay(100);
    
    // should be no more button clicks
    numLcdBacklightButtonPresses = 0;

    // do whatever we're supposed to do when button is pressed
    setBacklight(true);
    setLcdBacklightTimer();
    incrementLcdPage();   
  } else {
    checkLcdBacklightTimer();
  }
}

void setLcdBacklightTimer() {
   lcdBacklightCountdownTimer = millis() + 120000; 
}

void checkLcdBacklightTimer() {
    if (lcdBacklightCountdownTimer < millis()) {
      setBacklight(false);
      // set lcd page to last page when we turn backlight off
      // so that when button to turn on backlight is pressed
      // next time, it will increment current page and rollover
      // to first page
      lcdCurrentPage = LCD_MAX_PAGES;
    }
}

void incrementLcdPage() {
  lcdPreviousPage = lcdCurrentPage;
  lcdCurrentPage += 1;
  if (lcdCurrentPage > LCD_MAX_PAGES) {
    lcdCurrentPage = 1;
  }
}

// return a string filled with blanks
String getBlankLine(int length) {
  String blankLine;
  for (int x=0; x<length; x++) {
    blankLine += ' ';
  }
  return blankLine;
}

void clearLcdBuffer() {
  String blankLine = getBlankLine(lcdCols);
  for (int x=0; x<lcdRows; x++) {
    setLcdBufferRow(x, blankLine);
  }  
}

void lcdCheckPageChange() {
  if (lcdPreviousPage != lcdCurrentPage) {
    clearLcdBuffer();
    refreshLcd();
    lcdPreviousPage = lcdCurrentPage;
  }
}

