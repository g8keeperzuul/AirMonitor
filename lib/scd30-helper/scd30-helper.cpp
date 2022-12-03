#include "scd30-helper.h"

// ------------------------------------------------------------------------------------------
// CO2 (and temperature & humidity)

Adafruit_SCD30 scd30;
//uint16_t scd30_last_pressure_compensation = 1; // automatically set from dps310 pressure data when scd30_ambient_pressure_compensation_enabled == true

/**
  Initialize the SCD30 CO2 sensor. 
  This sensor also has onboard temperature and humidity sensors used to calibrate the CO2 sensor. 
  They do not appear to be as accurate as the SHT40 which is used as the primary temperature and humidity sensor.

  Use the temperature offset function to bring the SCD30 onboard temperature sensor in line with the SHT40. 
  Oddly the offset only accepts positive values, so I'm not sure how compensating for too low a reading is done.
  Maybe this is not required if the compensation is meant to offset the heating element on the sensor.

  The following parameters can be set via MQTT configuration messages so they are not initialized here:
    + temperature offset
    + altitude offset
  Their configuration is stored in non-volatile memory on the SCD30 sensor.
*/
bool initSCD30(){
  Sprint(F("Initialize SCD30 sensor... "));
  if(scd30.begin()) {
    Sprintln(F("OK"));
    return true;
  }
  else{
    Sprintln("Failed to find SCD30!");
    return false;
  }  

  /*** Adjust the rate at which measurements are taken, from 2-1800 seconds */
  scd30.setMeasurementInterval(25);
  Sprint(F("SCD30 measurement interval (seconds): "));
  Sprintln(scd30.getMeasurementInterval());  

  /*** Set a temperature offset in hundredths of a degree celcius.
   * Offset value stored in non-volatile memory of SCD30.
   * It is also a retained value on the MQTT topic when initially set (homeassistant/sensor/environmental/featherm0/temperature_offset/get)
   * scd30.setTemperatureOffset(1984) == 19.84 degrees celcius
   */
  Sprint(F("SCD30 temperature offset (C): "));
  Sprintln((float)scd30.getTemperatureOffset()/100.0);    

  /*** Set an altitude offset in meters above sea level.
   * Offset value stored in non-volatile memory of SCD30.
   * Setting an altitude offset will override any pressure offset.
   */
  Sprint(F("SCD30 altitude offset (meters): "));
  Sprintln(scd30.getAltitudeOffset());  

  /***
    Ambient pressure offset takes precedence over altitude offset as long as pressure offset != 0
  */
  Sprint(F("SCD30 ambient pressure offset (mBar/hPa): "));
  Sprintln(scd30.getAmbientPressureOffset());  // can only be set with continuous measurement method 

  /*** Enable or disable automatic self calibration (ASC).
   * Parameter stored in non-volatile memory of SCD30.
   * Enabling self calibration will override any previously set
   * forced calibration value.
   * ASC needs continuous operation with at least 1 hour
   * 400ppm CO2 concentration daily.
   * Automatic self calibration requires about 5 days in total with some time outdoors. 
   * For this reason it is DISABLED.
   * https://learn.adafruit.com/adafruit-scd30/field-calibration
   */
  scd30.selfCalibrationEnabled(false);

  Sprint(F("SCD30 automatic self calibration (ASC): "));
  if (scd30.selfCalibrationEnabled()) {
    Sprintln(F("ENABLED"));
  } else {
    Sprintln(F("DISABLED"));
  }  

  /***
    Forced calibration reference (FCR) resets to 400 on device reset
    However, this does not mean that it needs to be recalibrated every time the device is restarted.
    Even though the FCR appears to be reset, the new baseline is indeed remembered by the sensor and repeated recalibration is unnecessary.
    The last FCR value is retained in the MQTT topic (homeassistant/sensor/environmental/featherm0/co2_reference/get) and does survive device resets.
  */
  Sprint(F("SCD30 forced calibration reference (baseline CO2 ppm): "));
  Sprintln(scd30.getForcedCalibrationReference());
}

/**
  Start continuous measurement to measure CO2 concentration, relative humidity and temperature 
  or updates the ambient pressure if the periodic measurement is already running.

  Measurement data which is not read from the sensor is continuously
  overwritten. The CO2 measurement value can be compensated for ambient
  pressure by setting the pressure value in mBar. Setting the ambient pressure
  overwrites previous and future settings of altitude compensation. Setting the
  pressure to zero deactivates the ambient pressure compensation.
  The continuous measurement status is saved in non-volatile memory. The last
  measurement mode is resumed after repowering.

  ambient_pressure_mbar Ambient pressure in millibars. 0 to deactivate
                               ambient pressure compensation (reverts to
                               altitude compensation, if set), 700-1200mBar
                               allowable range otherwise  
  return                      boolean to indicate success
*/    
bool configSCD30PressureOffset(uint16_t ambient_pressure_mbar){
  if(!scd30.startContinuousMeasurement(ambient_pressure_mbar)){ // 0 if the command was successful else an error code
      Sprint(F("Failed to set SCD30 ambient pressure offset to (mBar/hPa): ")); Sprintln(ambient_pressure_mbar);
      return false;
  }

  Sprint(F("SCD30 ambient pressure offset (mBar/hPa): "));
  Sprintln(scd30.getAmbientPressureOffset());  // can only be set with continuous measurement method 
  return true;
}

/**
  Sets altitude offset.
*/
bool setAltitudeOffset(long alt_offset){
  Sprint(F("Calibrating altitude offset... "));

  bool alt_offset_success = scd30.setAltitudeOffset(alt_offset); // setting survives restarts
  if(alt_offset_success){
    Sprint(F("OK"));           
  }
  else{
    Sprint(F("FAILED!"));    
  }
  Sprint(F("SCD30 altitude offset (meters): "));
  Sprintln(scd30.getAltitudeOffset());
  return alt_offset_success;
}

/**
  Sets temperature offset. 
*/
bool setTemperatureOffset(int temp_offset){
    Sprint(F("Calibrating temperature offset... "));
    bool temp_offset_success = scd30.setTemperatureOffset(temp_offset); // setting survives restarts
    if(temp_offset_success){
        Sprintln(F("OK"));        
    }
    else{
        Sprintln(F("FAILED!"));
    }
    Sprint(F("SCD30 temperature offset (one-hundredths of a degree celcius): "));
    Sprintln(scd30.getTemperatureOffset());      

    return temp_offset_success;  
}

/**
  Sets the new forced CO2 calibration baseline value.
  If you already have another calibrated CO2 sensor, then use that value with devices side-by-side.
  Otherwise move to outdoor air and set reference to about 430 ppm.
*/
bool setCO2Reference(int co2_ref){
    Sprintln(F("Calibrating CO2 baseline... "));

    bool co2ref_success = scd30.forceRecalibrationWithReference(co2_ref); // setting survives restarts
    if(co2ref_success){
        Sprintln(F("OK")); 
    }
    else{
        Sprintln(F("FAILED!"));  
    }
    Sprint(F("SCD30 forced CO2 calibration reference (ppm): "));
    Sprintln(scd30.getForcedCalibrationReference());    
    return co2ref_success;   
}

scd30_data readSCD30(){

  scd30_data data;
  data.temperature = 0.0F;
  data.relative_humidity = -1.0F;
  data.CO2 = -1.0F;

  if (scd30.dataReady()){
      if (!scd30.read()){ 
        Sprintln("Error reading SCD30 sensor data"); 
        return data; 
      }

      // Sprint("Temperature: ");
      // Sprint(scd30.temperature);
      // Sprintln(" degrees C");
      
      // Sprint("Relative Humidity: ");
      // Sprint(scd30.relative_humidity);
      // Sprintln(" %");
      
      // Sprint("CO2: ");
      // Sprint(scd30.CO2, 3);
      // Sprintln(" ppm");
      // Sprintln("");

      data.CO2 = scd30.CO2;
      data.temperature = scd30.temperature;
      data.relative_humidity = scd30.relative_humidity;

      return data;
  } 
  else {
    Sprintln("SCD30 sensor data not ready");    
    return data;
  }    
}
