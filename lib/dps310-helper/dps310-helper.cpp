#include "dps310-helper.h"

// ------------------------------------------------------------------------------------------
// pressure (and temperature)
Adafruit_DPS310 dps;

bool initDPS310(){  
  Sprint(F("Initialize DPS310 sensor..."));
  if(dps.begin_I2C()) {
    Sprintln(F("OK"));
    return true;
  }
  else{
    Sprintln("Failed to find DPS310!");
    return false;
  }
  
  // Setup highest precision - this is the default configuration (see Adafruit_DPS310::_init(void))
  //dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  //dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);

  // Also automatically done by default (see Adafruit_DPS310::_init(void))
  // https://github.com/adafruit/Adafruit_DPS310/blob/master/Adafruit_DPS310.h#L63
  //dps.setMode(DPS310_CONT_PRESTEMP);

  #ifndef DISABLE_SERIAL_OUTPUT
  Adafruit_Sensor *dps_temp = dps.getTemperatureSensor();
  Adafruit_Sensor *dps_pressure = dps.getPressureSensor();  

  dps_temp->printSensorDetails();
  dps_pressure->printSensorDetails();
  #endif
}

dps310_data readDPS310(){
  sensors_event_t temp_event, pressure_event;
  dps310_data data;
  data.temperature = 0.0F;
  data.pressure = 0.0F;
  data.altitude = 0.0F;
  Adafruit_Sensor *dps_temp = dps.getTemperatureSensor();
  Adafruit_Sensor *dps_pressure = dps.getPressureSensor();
  
  if (true /*dps.temperatureAvailable()*/) {
    dps_temp->getEvent(&temp_event);
    data.temperature = temp_event.temperature;        
    //Sprint(F("Temperature = "));  Sprint(temp_event.temperature);  Sprintln(" *C");
  }

  // Reading pressure also reads temp so don't check pressure before temp!
  if (true /*dps.pressureAvailable()*/) {
    dps_pressure->getEvent(&pressure_event);
    data.pressure = pressure_event.pressure;    
    //Sprint(F("Pressure = "));  Sprint(pressure_event.pressure);  Sprintln(" hPa"); 
    data.altitude = dps.readAltitude(LOCAL_SEALEVEL_HPA);
  }  
  return data;   
}
