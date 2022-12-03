#include "sht40-helper.h"

// ------------------------------------------------------------------------------------------
// temperature and humidity

Adafruit_SHT4x sht4 = Adafruit_SHT4x();

bool initSHT40(){
  Sprint(F("Initialize SHT40 sensor... "));
  if (sht4.begin()) {

    Sprintln(F("OK"));

    Sprint("SHT40 serial number 0x");
    SprintlnHEX(sht4.readSerial());

    // You can have 3 different precisions, higher precision takes longer
    Sprint(F("SHT40 Precision: "));  
    sht4.setPrecision(SHT4X_HIGH_PRECISION);
    switch (sht4.getPrecision()) {
      case SHT4X_HIGH_PRECISION: 
        Sprintln(F("High"));
        break;
      case SHT4X_MED_PRECISION: 
        Sprintln(F("Medium"));
        break;
      case SHT4X_LOW_PRECISION: 
        Sprintln(F("Low"));
        break;
    }

    // You can have 6 different heater settings
    // higher heat and longer times uses more power
    // and reads will take longer too!
    Sprint(F("SHT40 Heater: "));
    sht4.setHeater(SHT4X_NO_HEATER);
    switch (sht4.getHeater()) {
      case SHT4X_NO_HEATER: 
        Sprintln(F("not enabled"));
        break;
      case SHT4X_HIGH_HEATER_1S: 
        Sprintln(F("High heat for 1s"));
        break;
      case SHT4X_HIGH_HEATER_100MS: 
        Sprintln(F("High heat for 0.1s"));
        break;
      case SHT4X_MED_HEATER_1S: 
        Sprintln(F("Medium heat for 1s"));
        break;
      case SHT4X_MED_HEATER_100MS: 
        Sprintln(F("Medium heat for 0.1s"));
        break;
      case SHT4X_LOW_HEATER_1S: 
        Sprintln(F("Low heat for 1s"));
        break;
      case SHT4X_LOW_HEATER_100MS: 
        Sprintln(F("Low heat for 0.1s"));
        break;
    }
    return true;
  }
  else{
    Sprintln("Failed to find SHT40!");
    return false;
  } 
}

sht40_data readSHT40(){
  sensors_event_t humidity, temp;
  
  //uint32_t timestamp = millis();
  sht4.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  /*
  timestamp = millis() - timestamp;
  Sprint("Temperature: "); Sprint(temp.temperature); Sprintln(" degrees C");
  Sprint("Humidity: "); Sprint(humidity.relative_humidity); Sprintln("% rH");
  Sprint("Read duration (ms): "); Sprintln(timestamp);
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
