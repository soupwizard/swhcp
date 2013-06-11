
#define TEMPERATURE_PRECISION 9
#define CELCIUS true
#define FAHRENHEIT false

void setupOneWire() {
  
  // Start up the library
  oneWireSensors.begin();

  // locate devices on the bus
  Serial.println("Locating Dallas One-Wire devices");
  numOneWireDevicesFound = oneWireSensors.getDeviceCount();
  
  Serial.print("Found ");
  Serial.print(numOneWireDevicesFound);
  Serial.println(" devices.");

  if (numOneWireDevicesFound < 1) { return; };

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (oneWireSensors.isParasitePowerMode()) {
    Serial.println("ON");
  } else {
    Serial.println("OFF");
  }

  // assign address manually.  the addresses below will beed to be changed
  // to valid device addresses on your bus.  device address can be retrieved
  // by using either oneWire.search(deviceAddress) or individually via
  // sensors.getAddress(deviceAddress, index)
  //oneWireAddr[0] = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
  //oneWireAddr[1] = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };
  
  // blue data wire
  //oneWireAddr[0] = { 0x28, 0xBA, 0xE3, 0x71, 0x04, 0x00, 0x00, 0xFB };

  // yellow data wire
  //oneWireAddr[0] = { 0x28, 0xA4, 0x6E, 0x5C, 0x04, 0x00, 0x00, 0x79 };

  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices, 
  // or you have already retrieved all of them.  It might be a good idea to 
  // check the CRC to make sure you didn't get garbage.  The order is 
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to insideThermometer
  //if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");
  // assigns the seconds address found to outsideThermometer
  //if (!oneWire.search(outsideThermometer)) Serial.println("Unable to find address for outsideThermometer");

  Serial.println("finding all devices");
  findAllDeviceAddresses(oneWireAddr);
  Serial.println("setting all temperature resolutions");
  //setAllTempResolutions(oneWireAddr, TEMPERATURE_PRECISION);
  Serial.println("finding all temperatures");
  findAllDeviceTemps(oneWireAddr, oneWireTemp);
  Serial.println("dumping all device info");
  String foo = dumpAllDeviceInfo(oneWireAddr);
  Serial.println(foo);
  //Serial.println("printing all device info");
  //printOneWireData(oneWireAddr[0]);
  //printOneWireData(oneWireAddr[1]);
}

void findAllDeviceAddresses(DeviceAddress oneWireAddr[]) {
    for (int x=0; x<ONE_WIRE_MAX_NUM_DEVICES; x++) {
        if (!oneWireSensors.getAddress(oneWireAddr[x], x)) {
          //Serial.print("Unable to find address for Device "); 
          //Serial.print(x);
        }
    }
}

void setAllTempResolutions(DeviceAddress oneWireAddr[], int resolution) {
  for (int x=0; x<ONE_WIRE_MAX_NUM_DEVICES; x++) {
    oneWireSensors.setResolution(oneWireAddr[x], resolution);
  }
}

int getTempResolution(DeviceAddress deviceAddress) {
  return oneWireSensors.getResolution(deviceAddress); 
}

void findAllDeviceTemps(DeviceAddress oneWireAddr[], float oneWireTemp[]) {
    for (int x=0; x<ONE_WIRE_MAX_NUM_DEVICES; x++) {
      oneWireTemp[x] = getOneWireTemperature(oneWireAddr[x], FAHRENHEIT);
      Serial.print("got temp: ");
      Serial.println(oneWireTemp[x]);
      
    }
}

String dumpAllDeviceInfo(DeviceAddress oneWireAddr[]) {
  String deviceInfo = "";  
  for (int x=0; x<ONE_WIRE_MAX_NUM_DEVICES; x++) {
    deviceInfo += dumpDeviceInfo(oneWireAddr[x]) + "\n";
  }  
  return deviceInfo;
}

String dumpDeviceInfo(DeviceAddress deviceAddr) {
  String deviceInfo = "Addr: " + getOneWireAddress(deviceAddr) + 
                      ", Resolution: " + (String) getTempResolution(deviceAddr) + 
                      ", Temp(C): " + getTemperatureStr(deviceAddr);
  return deviceInfo;
}

String getTemperatureStr(DeviceAddress deviceAddr) {
  float tempValue = getOneWireTemperature(deviceAddr, FAHRENHEIT);
  char tempStr[16];
  dtostrf(tempValue, 1, 1, tempStr);
  Serial.print("*** got temp ");
  Serial.println(tempStr);
  return (String) tempStr;
}

String oneWireFloatToString(float temp) {
  char tempStr[16];
  dtostrf(temp, 1, 1, tempStr);
  return tempStr;
}


// function to print a device address
String getOneWireAddress(DeviceAddress deviceAddress) {
  String deviceAddr = "";
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) { deviceAddr += "0"; }
    deviceAddr += String(deviceAddress[i], HEX);   
  }
  return deviceAddr;
}

float getOneWireTemperature(DeviceAddress deviceAddress, boolean getCelcius) {
  float temp;
  if (getCelcius) {
    temp = oneWireSensors.getTempC(deviceAddress);
  } else {
    temp = oneWireSensors.getTempF(deviceAddress);
  } 
  return temp;
}

// function to print a device address
String printOneWireAddress(DeviceAddress deviceAddress) {
  String deviceAddr = "";
  for (uint8_t i = 0; i < 8; i++) {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) { Serial.print("0"); }
    Serial.print(deviceAddress[i], HEX);
  }
}

// main function to print information about a device
void printOneWireData(DeviceAddress deviceAddress) {
  Serial.print("Device Address: ");
  printOneWireAddress(deviceAddress);
  Serial.print(", Resoultion: ");
  Serial.print(oneWireSensors.getResolution(deviceAddress));  
  Serial.print(", Temp: ");
  Serial.print(getOneWireTemperature(deviceAddress, FAHRENHEIT));
  Serial.print(" (");
  Serial.print(getOneWireTemperature(deviceAddress, CELCIUS));
  Serial.print(")");
  Serial.println();
}


