#ifndef SHT40_HELPER_H
#define SHT40_HELPER_H

#include <string>
#include <Adafruit_SHT4x.h>
#include "log.h"


// *********************************************************************************************************************
// *** Data Types ***
struct sht40_data{
    float temperature; /** temperature is in degrees centigrade (Celsius) */
    float relative_humidity; /** relative humidity in percent */
};

struct sht40_config{
    std::string heater_intensity; /** Disabled | High | Medium | Low | UNKNOWN */
    uint16_t heater_duration; /** milliseconds */
    std::string precision; /** High | Medium | Low */
};

// *********************************************************************************************************************
// *** Must Declare ***
extern Adafruit_SHT4x sht4;

// *********************************************************************************************************************
// *** Must Implement ***
bool initSHT40();
sht40_data readSHT40();
sht40_config readSHT40Config();

#endif