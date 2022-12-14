#ifndef PM25_HELPER_H
#define PM25_HELPER_H

#include <Adafruit_PM25AQI.h>
#include "log.h"

// *********************************************************************************************************************
// *** Data Types ***

// *********************************************************************************************************************
// *** Must Declare ***
extern Adafruit_PM25AQI aqi;

// *********************************************************************************************************************
// *** Must Implement ***
bool initPM25AQI();
PM25_AQI_Data readPM25AQI();
uint16_t calculateAQI(uint16_t conc_i);

#endif