#include "sht40-helper.h"

// ------------------------------------------------------------------------------------------
// temperature and humidity

Adafruit_SHT4x sht4 = Adafruit_SHT4x();

bool initSHT40(){
  Serial.print(F("Initialize SHT40 sensor... "));
  if (sht4.begin()) {

    Serial.println(F("OK"));

    Serial.print("SHT40 serial number 0x");
    Serial.println(sht4.readSerial(), HEX);

    // You can have 3 different precisions, higher precision takes longer
    Serial.print(F("SHT40 Precision: "));  
    sht4.setPrecision(SHT4X_HIGH_PRECISION);
    switch (sht4.getPrecision()) {
      case SHT4X_HIGH_PRECISION: 
        Serial.println(F("High"));
        break;
      case SHT4X_MED_PRECISION: 
        Serial.println(F("Medium"));
        break;
      case SHT4X_LOW_PRECISION: 
        Serial.println(F("Low"));
        break;
    }

    // You can have 6 different heater settings
    // higher heat and longer times uses more power
    // and reads will take longer too!
    Serial.print(F("SHT40 Heater: "));
    sht4.setHeater(SHT4X_NO_HEATER);
    switch (sht4.getHeater()) {
      case SHT4X_NO_HEATER: 
        Serial.println(F("not enabled"));
        break;
      case SHT4X_HIGH_HEATER_1S: 
        Serial.println(F("High heat for 1s"));
        break;
      case SHT4X_HIGH_HEATER_100MS: 
        Serial.println(F("High heat for 0.1s"));
        break;
      case SHT4X_MED_HEATER_1S: 
        Serial.println(F("Medium heat for 1s"));
        break;
      case SHT4X_MED_HEATER_100MS: 
        Serial.println(F("Medium heat for 0.1s"));
        break;
      case SHT4X_LOW_HEATER_1S: 
        Serial.println(F("Low heat for 1s"));
        break;
      case SHT4X_LOW_HEATER_100MS: 
        Serial.println(F("Low heat for 0.1s"));
        break;
    }
    return true;
  }
  else{
    Serial.println("Failed to find SHT40!");
    return false;
  } 
}

sht40_data readSHT40(){
  sensors_event_t humidity, temp;
  
  //uint32_t timestamp = millis();
  sht4.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  /*
  timestamp = millis() - timestamp;
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
  Serial.print("Read duration (ms): "); Serial.println(timestamp);
  */
  sht40_data data;
  data.temperature = temp.temperature;
  data.relative_humidity = humidity.relative_humidity;
  
  return data;  
}

sht40_config readSHT40Config(){

  sht40_config config;

  switch (sht4.getHeater()) { 
     case SHT4X_NO_HEATER:  
       config.heater_intensity = "Disabled";
       config.heater_duration = 0;   
       break; 
     case SHT4X_HIGH_HEATER_1S:  
       config.heater_intensity = "High";
       config.heater_duration = 1000;
       break; 
     case SHT4X_HIGH_HEATER_100MS:  
       config.heater_intensity = "High"; 
       config.heater_duration = 100;
       break; 
     case SHT4X_MED_HEATER_1S:  
       config.heater_intensity = "Medium"; 
       config.heater_duration = 1000;
       break; 
     case SHT4X_MED_HEATER_100MS:  
       config.heater_intensity = "Medium"; 
       config.heater_duration = 100;
       break; 
     case SHT4X_LOW_HEATER_1S:  
       config.heater_intensity = "Low"; 
       config.heater_duration = 1000;
       break; 
     case SHT4X_LOW_HEATER_100MS:  
       config.heater_intensity = "Low"; 
       config.heater_duration = 100;
       break;  
     default:    
       config.heater_intensity = "UNKNOWN"; 
       config.heater_duration = 9999;     
  } 
  
  switch (sht4.getPrecision()) { 
     case SHT4X_HIGH_PRECISION:  
       config.precision = "High"; 
       break; 
     case SHT4X_MED_PRECISION:  
       config.precision = "Medium"; 
       break; 
     case SHT4X_LOW_PRECISION:  
       config.precision = "Low"; 
       break; 
  }   

  return config;
}
