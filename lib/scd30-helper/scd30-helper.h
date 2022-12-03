#ifndef SCD30_HELPER_H
#define SCD30_HELPER_H

#include <Adafruit_SCD30.h>
#include "log.h"

// *********************************************************************************************************************
// *** Data Types ***
struct scd30_data{
  float CO2; /** carbon dioxide in PPM */
  float temperature; /** temperature is in degrees centigrade (Celsius) */
  float relative_humidity; /** relative humidity in percent */
};

// *********************************************************************************************************************
// *** Must Declare ***
extern Adafruit_SCD30 scd30;

// *********************************************************************************************************************
// *** Must Implement ***
bool initSCD30();
bool configSCD30PressureOffset(uint16_t ambient_pressure_mbar);
bool setAltitudeOffset(long alt_offset);
bool setTemperatureOffset(int temp_offset);
bool setCO2Reference(int co2_ref);
scd30_data readSCD30();

#endif