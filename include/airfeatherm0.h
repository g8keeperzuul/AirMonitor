/*
    Contains hardware specific information for the base microcontroller board used. 
*/
#ifndef AIRFEATHERM0_H
#define AIRFEATHERM0_H

//#include <Arduino.h>  appears to be automatically added by framework specified in platformio.ini
#include <wifi-helper.h>
#include "mqtt-ha-helper.h"
#include "env.h"
#include "utils.h"
#include "sht40-helper.h"
#include "dps310-helper.h"
#include "pm25-helper.h"
#include "scd30-helper.h"
#include "log.h"

#define LED_OFF LOW
#define LED_ON HIGH

// Amount of time to delay before resetting device.
// Device is reset when an unrecoverable condition occurs (such as inability to initialize sensor).
#define RESET_DELAY (5*60*1000) // 5 minutes

#define DEVICE_ID "featherm0"
#define DEVICE_NAME "AirMonitor"
#define DEVICE_MANUFACTURER "Adafruit"
#define DEVICE_MODEL "Feather M0"
#define DEVICE_VERSION "20221202.1608"

// *********************************************************************************************************************
// *** Must Declare ***

#endif