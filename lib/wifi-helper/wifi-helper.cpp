#include "wifi-helper.h"

#define WIFI_ATTEMPT_COOLDOWN 30000 // milliseconds between connection attempts

std::string cached_mac;

bool initWifi() {
  Serial.print(F("Initalize wifi..."));
  
  WiFi.setPins(WINC_CS, WINC_IRQ, WINC_RST, WINC_EN);

  // check for the presence of the breakout
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println(F("WINC1500 not present. Abort"));
    return false;
  }
  Serial.println(F("ATWINC OK"));  
    
  return true;
}

/*
  Attempt to connect to wireless network ONCE, and wait ATTEMPT_DURATION (milliseconds) for 
  connection to be established. 
*/
bool connectWifi(const char *ssid, const char *passphrase)
{
  byte ledStatus = LOW;
  
  const uint16_t ATTEMPT_DURATION = 5000;
  uint16_t duration = 0;  

  // attempt to connect to Wifi network
  /*
      WiFi.status() options:

      WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
      WL_IDLE_STATUS      = 0,
      WL_NO_SSID_AVAIL    = 1,
      WL_SCAN_COMPLETED   = 2,
      WL_CONNECTED        = 3,
      WL_CONNECT_FAILED   = 4,
      WL_CONNECTION_LOST  = 5,
      WL_DISCONNECTED     = 6
  */   

  Serial.print(F("Attempting to connect to wireless network \""));
  Serial.print(ssid);
  Serial.print("\"... ");

  // WiFI.begin([ssid], [passkey]) initiates a WiFI connection
  // to the stated [ssid], using the [passkey] as a WPA, WPA2,
  // or WEP passphrase.    
  WiFi.begin(ssid, passphrase);    

  while (duration < ATTEMPT_DURATION && WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(LED_BUILTIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;

    // Delays allow the ESP8266 to perform critical tasks
    // defined outside of the sketch. These tasks include
    // setting up, and maintaining, a WiFi connection.
    delay(100); // calls yield
    duration = duration + 100;
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.println(F("Connected!"));
    return true;
  }
  else{
    Serial.println(F("Failed!"));
    return false;
  }
  
}

/*
  Continuously tries to connect to the wireless network.
  Will wait ATTEMPT_COOLDOWN milliseconds between attempts. 

  WILL BLOCK IF UNABLE TO CONNECT TO NETWORK!

  Returns false if existing connection used, true if new connection established
  Will immediately return FALSE if already connected.
*/
bool assertNetworkConnectivity(const char *ssid, const char *passphrase){    

  if(WiFi.status() ==  WL_CONNECTED) {
    return false; // already connected, no new connection established
  }
  else {
    do {
      if(!connectWifi(ssid, passphrase)){ // each attempt is allowed ATTEMPT_DURATION to establish a connection
        delay(WIFI_ATTEMPT_COOLDOWN);
      }    
    } 
    while (WiFi.status() != WL_CONNECTED);
    return true; // new connection established
  }
}

void printNetworkDetails()
{
  Serial.println("");
  Serial.println("Network details:");  

  Serial.print(F("WiFi firmware version: "));
  Serial.println(WiFi.firmwareVersion());  

  // SSID of the network you're attached to
  Serial.print(F("SSID: "));
  Serial.println(WiFi.SSID()); // String

  Serial.print(F("IP: "));
  std::string ip = getIP();
  Serial.println(ip.c_str()); 
  
  Serial.print(F("Subnet mask: "));
  std::string mask = uint32_to_ip(WiFi.subnetMask());
  Serial.println(mask.c_str());   
  
  Serial.print(F("Gateway IP: "));
  std::string gw = uint32_to_ip(WiFi.gatewayIP());
  Serial.println(gw.c_str());   
    
  Serial.print(F("MAC: "));
  const std::string mac = getMAC();
  Serial.println(mac.c_str());

  // MAC address of the router you're attached to
  Serial.print("BSSID: ");
  uint8_t bssid_buf[6];
  WiFi.BSSID(bssid_buf);
  const std::string bssid_string = uint8_to_hex_string(bssid_buf, 6);
  Serial.println(bssid_string.c_str());

  // Received signal strength
  Serial.print(F("RSSI: "));
  Serial.println(WiFi.RSSI()); // int8_t  

  // Wifi password
  // Serial.print("PSK: ");
  // Serial.println(WiFi.psk()); // String

  // Encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type: ");
  Serial.println(encryption, HEX);

  Serial.println("");
}


/*
  Since MAC is frequently used as a device unique identifier
  and does not change, cache it.
*/
std::string getMAC(){
  
  if(cached_mac.empty()){
    uint8_t mac_buf[6];
    WiFi.macAddress(mac_buf);
    cached_mac = uint8_to_hex_string(mac_buf, 6);    
  }
  return cached_mac;
}

std::string getIP(){
  return uint32_to_ip(WiFi.localIP()); // uint32
}

int getRSSI(){
  return WiFi.RSSI();
}