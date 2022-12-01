#ifndef DPS310_HELPER_H
#define DPS310_HELPER_H

#include <Adafruit_DPS310.h>

// Sea level equivalent ranges from 1013 to 1018 at this location and is seasonal and regional; annual average is 1015.95
// https://www.worldweatheronline.com/markham-weather-averages/ontario/ca.aspx
// mean sea-level pressure (MSLP) (which does not take into account regional variations) is 1013.25 hPa worldwide
#define LOCAL_SEALEVEL_HPA 1015.95F // hPa 

// *********************************************************************************************************************
// *** Data Types ***
struct dps310_data{
  float temperature; /** temperature is in degrees centigrade (Celsius) */
  float pressure;    /** pressure in hectopascal (hPa) */
  float altitude;    /** meters; this will vary (even at a stationary location) with high and low pressure changes; only accurate at neutral pressure */
};

// *********************************************************************************************************************
// *** Must Declare ***
extern Adafruit_DPS310 dps;

// *********************************************************************************************************************
// *** Must Implement ***
bool initDPS310();
dps310_data readDPS310();

#endif