void setupDHT() {
  #ifdef MOCK_EVERYTHING
  #else
    dhtInside.begin();
    dhtOutside.begin();
  #endif
}

float getDhtTemperature(DHT dht, String dhtName) {

  #ifdef MOCK_EVERYTHING
    if (DHT_USE_FAHRENHEIT) {
      return 72.0;
    } else {
      return 26.0;
    } 
  #endif
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float t = dht.readTemperature(DHT_USE_FAHRENHEIT);

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if ( isnan(t) ) {
    #ifdef PRINT_SERIAL 
      Serial.print(" - Failed to read from DHT ");
      Serial.println(dhtName);
    #endif
    // should set an global warning flag here, since this isn't used in any calculations
    return 0.0;
  } else {
    #ifdef PRINT_SERIAL 
      Serial.print("DHT ");
      Serial.print(dhtName);
      Serial.print("Temperature: "); 
      Serial.print(t);
      Serial.println(" *C");
    #endif
    return t;
  }  
}

float getDhtHumidity(DHT dht, String dhtName) {

  #ifdef MOCK_EVERYTHING
    return 44.0; 
  #endif
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if ( isnan(h) ) {
    #ifdef PRINT_SERIAL
      Serial.print(" - Failed to read from DHT ");
      Serial.println(dhtName);
    #endif
    // should set an global warning flag here, since this isn't used in any calculations
    return 0.0;
  } else {
    #ifdef PRINT_SERIAL
      Serial.print("DHT ");
      Serial.print(dhtName);
      Serial.print(" - Humidity: "); 
      Serial.print(h);
    #endif
    return h;
  }  
}

String dhtFloatToStr(float value) {
  char tempStr[16];
  dtostrf(value, 1, 1, tempStr);
  return (String) tempStr;
}

