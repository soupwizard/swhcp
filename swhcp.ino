
/*
 * Solar Water Heater Project v2.0, Jeff Davis soupwizard@gmail.com
 *
 */

#include <DHT.h>                 // For DHT11/21/22
#include <OneWire.h>             // For Dallas DS18B20
#include <DallasTemperature.h>   // For Dallas DS18B20
#include <Wire.h>                // For RTC
#include <RTClib.h>              // For RTC
#include <LiquidCrystal_I2C.h>   // LCD 2004 or 1602 screen
#include <stdio.h>
#include <stdlib.h>
#include <MemoryFree.h>

/************************************************************************************
 ***                                                                              ***
 *** Pin Usage Defines                                                            ***
 ***                                                                              ***
 ************************************************************************************/

// pin usage defs for Mega 2560

#define SENSOR_FLOW_PANEL_LOOP  A8
#define SENSOR_FLOW_FLOOR_LOOP  A9
#define SENSOR_FLOW_HOUSE_LOOP  A10

#define PIN_BUTTON_LCD_BACKLIGHT  3
#define PIN_LED_ONBOARD	         13
#define PIN_I2C_SDA              20
#define PIN_I2C_SCL              21
#define PIN_ONE_WIRE_BUS         22  
#define PIN_DHT11_INSIDE         30   
#define PIN_DHT11_OUTSIDE        31  

#define PIN_RELAY_PUMP_SOLAR     36 
#define PIN_RELAY_PUMP_FLOOR     37
#define PIN_RELAY_PUMP_HOUSE     38

#define INTERRUPT_BUTTON_LCD_BACKLIGHT 1


/************************************************************************************
 ***                                                                              ***
 *** Program Globals                                                              ***
 ***                                                                              ***
 ************************************************************************************/

unsigned long loopCtr = 0;  // loop counter
String serialCmd = "";


/************************************************************************************
 ***                                                                              ***
 *** Component Defines                                                            ***
 ***                                                                              ***
 ************************************************************************************/

/*********************************
 *** Dallas One-Wire & DS18B20 ***
 *********************************/

OneWire oneWireTempBus(PIN_ONE_WIRE_BUS);           // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature oneWireSensors(&oneWireTempBus);     // Pass our oneWire reference to Dallas Temperature. 
int numOneWireDevicesFound = 0;
#define ONE_WIRE_MAX_NUM_DEVICES 1
DeviceAddress oneWireAddr[ONE_WIRE_MAX_NUM_DEVICES];
float oneWireTemp[ONE_WIRE_MAX_NUM_DEVICES];

/*******************************
 *** DHTxx Temp and Humidity ***
 *******************************/
// Avail types are: DHT11, DHT21, DHT22, AM2301

#define DHT_USE_FAHRENHEIT true

#define DHT_INSIDE_NAME "Inside"
DHT dhtInside(PIN_DHT11_INSIDE, DHT11);
float insideTemperature;
float insideHumidity;

#define DHT_OUTSIDE_NAME "Outside"
DHT dhtOutside(PIN_DHT11_OUTSIDE, DHT11);
float outsideTemperature;
float outsideHumidity;

/**************
 *** Relays ***
 **************/
int allRelayPins[] = { PIN_RELAY_PUMP_SOLAR, PIN_RELAY_PUMP_FLOOR, PIN_RELAY_PUMP_HOUSE };
int numRelays = sizeof(allRelayPins)/sizeof(int);

/*****************************
 *** RTC - Real Time Clock ***
 *****************************/
 RTC_DS1307 RTC;
 DateTime now;
 boolean rtcAvail = false;

/************
 *** LEDs ***
 ************/
int ledOnboardLit = false;  // onboard LED (pin 13) will be toggled high or low each time loop() is called

/****************
 *** LCD 2004 ***
 ****************/
int lcdRows = 4;
int lcdCols = 20;
String lcdBuffer[] = {"", "", "", ""};  // initalized to max number of rows of any lcd screen we can handle 
#define lcdAddr 0x20 // set the address of the I2C device the LCD is connected to
// create an lcd instance with correct constructor for how the lcd is wired to the I2C chip
LiquidCrystal_I2C lcd(lcdAddr, 4, 5, 6, 0, 1, 2, 3, 7, NEGATIVE); // addr, EN, RW, RS, D4, D5, D6, D7, Backlight, POLARITY
int numLcdBacklightButtonPresses;  // for button that turns backlight on
unsigned long lcdBacklightCountdownTimer = millis();
#define LCD_PAGE_DIAG 1
#define LCD_PAGE_TEMPS 2
#define LCD_MAX_PAGES 2
int lcdCurrentPage  = LCD_PAGE_DIAG; 
int lcdPreviousPage = LCD_PAGE_DIAG;


/************************************************************************************
 ***                                                                              ***
 *** Arduino Library Required Function                                            ***
 ***                                                                              ***
 ************************************************************************************/

void setup()
{  
  // Setup relays first since they default to ON
  setupRelays();
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // this check is only needed on the Leonardo:
  while (!Serial) {}  // wait for Leonardo

  // RTC
  setupRTC();
  // rtc is not connected, set time
  //setDateTime();
  
  // Switches and Buttons
  pinMode(PIN_BUTTON_LCD_BACKLIGHT, INPUT);
  attachInterrupt(INTERRUPT_BUTTON_LCD_BACKLIGHT, handleLcdBacklightButton, RISING);

  // LCD Screen
  setupLcd();

  // Setup and print info about SD Card
  //sdcardAvail = (setupSdCard(PIN_SPI_SELECT_SDCARD) == 0) ? true : false;

  // Setup LEDs
  pinMode(PIN_LED_ONBOARD, OUTPUT);
  toggleLedOnboard();

  // Setup dallas one-wire sensors
  //setupOneWire();
  
  // Setup DHTxx devices
  setupDHT();

}

/**************************************************************/

void loop()
{
  loopCtr += 1;
  
  toggleLedOnboard();
  
  checkLcdBacklightButton();

  // Lcd screen will flicker if updated too often
  // put this on a timer interrupt, time set var to "update lcd"?
  
  // Every x loops thru code, update lcd
  if (loopCtr % 1 == 0) {
    now = RTC.now();
    
    lcdCheckPageChange();
    
    if (lcdCurrentPage == LCD_PAGE_DIAG) {
      setLcdBufferRow(0, (String) (getDateStr(now) + " " + getTimeStr(now)));
      setLcdBufferRow(1, "*" + serialCmd + "*  JAVA SUX");
      setLcdBufferRow(2, (String) lcdPreviousPage + " " + lcdCurrentPage);
      setLcdBufferRow(3, "FM: " + (String) freeMemory() + " " + lcdCurrentPage);
    } else {
      setLcdBufferRow(0, (String) (getDateStr(now) + " " + getTimeStr(now)));
      setLcdBufferRow(1, dhtFloatToStr(insideTemperature) + " " + dhtFloatToStr(insideHumidity));
      //setLcdBufferRow(2, oneWireFloatToString(oneWireTemp[0]) );
      setLcdBufferRow(3, (String) lcdCurrentPage);
    }

    refreshLcd();  
  }
  
  checkSensors();
      
  // Make changes based on inputs
  //chnageOutputs();
      
  // Send info to to web storage 
  serialCmd = "";
  String readString = "";
  while (Serial.available()) {  
    delay(2);  //delay to allow buffer to fill 
    if (Serial.available() > 0) {
      char c = Serial.read();  //gets one byte from serial buffer
      readString += c; //makes the string readString
    } 
  }
    
  if (readString.length() > 0) {
    serialCmd = readString;
    if (serialCmd == "GU") {
      serialSendStatus();
    }
  }
  
  
  delay(100);

}


/************************************************************************************
 ***                                                                              ***
 *** Program Functions                                                            ***
 ***                                                                              ***
 ************************************************************************************/

void checkSensors() {
  // Read temp sensors
  
  /* DS18B20 Temps */
  
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  //Serial.print("Requesting one-wire temperatures...");
  //oneWireSensors.requestTemperatures();
  //findAllDeviceTemps(oneWireAddr, oneWireTemp);
  //getDhtTemperature(DHT dht, String dhtName)
  //findAllDeviceTemps(oneWireAddr, oneWireTemp);
  //printOneWireData(oneWireAddr[0]);
  
  //Serial.println("Requesting DHT temperatures..");
  
  /* DHTxx Temps */
  // inside
  insideTemperature = getDhtTemperature(dhtInside, DHT_INSIDE_NAME);
  insideHumidity    = getDhtHumidity(dhtInside, DHT_INSIDE_NAME);
  // outside
  //outsideTemperature = getDhtTemperature(dhtOutside, DHT_OUTSIDE_NAME);
  //outsideHumidity    = getDhtHumidity(dhtOutside, DHT_OUTSIDE_NAME);
}

void chnageOutputs() {
  // eventually here is where pumps turn on etc 
  
}

void serialSendStatus() {
  
// ascii table constants
#define ASCII_HEX_STX  0x02
#define ASCII_HEX_ETX  0x03 
#define ASCII_HEX_US   0x1F
#define ASCII_HEX_RS   0x1E
  
  now = RTC.now();
  
  Serial.write(ASCII_HEX_STX);
  Serial.print('\n');
  
  Serial.print("DT");
  Serial.print('\t');
  Serial.print(getDateStr(now));
  Serial.print('\n');

  Serial.print("TM");
  Serial.print('\t');
  Serial.print(getTimeStr(now));
  Serial.print('\n');
  
  Serial.print("FM");
  Serial.print('\t');
  Serial.print(freeMemory());
  Serial.print('\n');
  
  Serial.print("IT");
  Serial.print('\t');
  Serial.print(insideTemperature);
  Serial.print('\n');  
  
  Serial.print("IH");
  Serial.print('\t');
  Serial.print(insideHumidity);
  Serial.print('\n');  
  
  Serial.write(ASCII_HEX_ETX);
  Serial.print('\n');

}



void flashLed(int pin, int ms) {
  digitalWrite(pin, HIGH);
  delay(ms);
  digitalWrite(pin, LOW);
  delay(ms);
}

void toggleLedOnboard() {
  switch (ledOnboardLit) {
    case true: 
      ledOnboardLit = false;
      digitalWrite(PIN_LED_ONBOARD, LOW);
      break;
    case false:
      ledOnboardLit = true;
      digitalWrite(PIN_LED_ONBOARD, HIGH);
      break;
    default:
      break;
  }
}


