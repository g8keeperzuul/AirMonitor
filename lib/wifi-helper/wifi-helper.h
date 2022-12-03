#ifndef WIFI_HELPER_H
#define WIFI_HELPER_H

#include <string>
#include <SPI.h>  // specific to WiFi101
#include <WiFi101.h>
#include "utils.h"
#include "log.h"

#define WINC_CS   8
#define WINC_IRQ  7
#define WINC_RST  4
#define WINC_EN   2     // or, tie EN to VCC

// *********************************************************************************************************************
// *** Must Declare ***
// n/a

// *** Must Implement ***
bool initWifi();
bool connectWifi(const char *ssid, const char *passphrase);
bool assertNetworkConnectivity(const char *ssid, const char *passphrase);
void printNetworkDetails();
std::string getMAC();
std::string getIP();
int getRSSI();

// Implement in main to provide customized handler
void indicateWifiProblem(byte return_code);

#endif