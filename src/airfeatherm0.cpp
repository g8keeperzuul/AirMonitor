//#include <Arduino.h>  appears to be automatically added by framework specified in platformio.ini
#include <wifi-helper.h>
#include "mqtt-ha-helper.h"
#include "airfeatherm0.h"
#include "env.h"
#include "utils.h"
#include "sht40-helper.h"
#include "dps310-helper.h"
#include "pm25-helper.h"
#include "scd30-helper.h"

/*
Built for Adafruit Feather M0 with ATWINC1500 wireless module 
https://www.adafruit.com/product/3010
*/

// *** Global Variables ***
WiFiClient wificlient;
MQTTClient mqttclient(768); // default is 128 bytes;  https://github.com/256dpi/arduino-mqtt#notes

/*
  Since the fully constructed list of discovery_config (topics and payloads) consumes considerable RAM, reduce it to just the facts.
  Generate each discovery_config one at a time at the point of publishing the message in order to conserve RAM.
  Once everything is successfully published, purge list contents (no longer needed).
*/
std::vector<discovery_metadata> discovery_metadata_list;                                            // list of data used to construct discovery_config for sensor discovery
std::vector<discovery_config_metadata> discovery_config_metadata_list;                              // list of data used to construct discovery_config for config/control discovery
std::vector<discovery_measured_diagnostic_metadata> discovery_measured_diagnostic_metadata_list;    // list of data used to construct discovery_config for sensor measured diagnostics (like RSSI)
std::vector<discovery_fact_diagnostic_metadata> discovery_fact_diagnostic_metadata_list;            // list of data used to construct discovery_config for fact diagnostics (like IP address)

// Last will and testament topic
const std::string AVAILABILITY_TOPIC = buildAvailabilityTopic("sensor", std::string(DEVICE_ID)); // homeassistant/sensor/featherm0/availability

const std::string DIAGNOSTIC_TOPIC = buildDiagnosticTopic("sensor", std::string(DEVICE_ID)); // homeassistant/sensor/featherm0/diagnostics

// All sensor updates are published in a single complex json payload to a single topic
const std::string STATE_TOPIC = buildStateTopic("sensor", std::string(DEVICE_ID)); // homeassistant/sensor/featherm0/state

// *** Control/Config Topics ***

// Temperature offset config value is set and retrieved via two dedicated topics. The message payload is just the value itself.
const std::string SET_TEMPOFFSET_TOPIC = buildSetterTopic("sensor", std::string(DEVICE_ID), "temperature_offset"); // homeassistant/sensor/environmental/featherm0/temperature_offset/set
const PROGMEM char *SUB_TEMPOFFSET_TOPIC = SET_TEMPOFFSET_TOPIC.c_str(); 
const std::string GET_TEMPOFFSET_TOPIC = buildGetterTopic("sensor", std::string(DEVICE_ID), "temperature_offset"); // homeassistant/sensor/environmental/featherm0/temperature_offset/get
const PROGMEM char *PUB_TEMPOFFSET_TOPIC = GET_TEMPOFFSET_TOPIC.c_str(); 

// *****************************


/*
  device_class : https://developers.home-assistant.io/docs/core/entity/sensor?_highlight=device&_highlight=class#available-device-classes
                 https://www.home-assistant.io/integrations/sensor/#device-class
  has_sub_attr : true if you want to provide sub attributes under the main attribute
  icon : https://materialdesignicons.com/
  unit : (depends on device class, see link above)
*/
// build discovery message - step 1 of 4
std::vector<discovery_metadata> getAllDiscoveryMessagesMetadata(){
  discovery_metadata temp, humidity, pressure, aqi, pm1, pm25, pm10, co2;

  // default device_type = "sensor"
  // default sub_attr = false

  temp.device_class = "temperature";  
  temp.icon = "mdi:home-thermometer";
  temp.unit = "°C";

  humidity.device_class = "humidity";  
  humidity.icon = "mdi:water-percent";
  humidity.unit = "%";

  pressure.device_class = "pressure";
  pressure.icon = "mdi:gauge";
  pressure.unit = "hPa";

  aqi.device_class = "aqi";
  aqi.has_sub_attr = true; 
  aqi.icon = "mdi:gauge";
  aqi.unit = "";

  pm1.device_class = "pm1";
  pm1.icon = "mdi:weather-dust";
  pm1.unit = "µg/m³";

  pm25.device_class = "pm25";
  pm25.icon = "mdi:weather-dust";
  pm25.unit = "µg/m³";

  pm10.device_class = "pm10";
  pm10.icon = "mdi:weather-dust";
  pm10.unit = "µg/m³";

  co2.device_class = "carbon_dioxide";
  co2.has_sub_attr = true; 
  co2.icon = "mdi:molecule-co2";
  co2.unit = "ppm";  

  std::vector<discovery_metadata> dm = { temp, humidity, pressure, aqi, pm1, pm25, pm10, co2 };  
  return dm;
}

// build discovery config/control message - step 1 of 4
std::vector<discovery_config_metadata> getAllDiscoveryConfigMessagesMetadata(){
  discovery_config_metadata tempoffset/*, useambpres, altoffset, co2ref, refrate*/;

  tempoffset.device_type = "number";
  tempoffset.control_name = "temperature_offset";
  tempoffset.custom_settings = "\"min\": 0, \"max\": 10000, \"step\": 10, \"initial\": 0";
  tempoffset.icon = "mdi:home-thermometer";
  tempoffset.unit = "hundredths °C";

  // useambpres.device_type = "switch";
  // useambpres.control_name = "use_ambient_pressure";
  // useambpres.custom_settings = "";
  // useambpres.icon = "mdi:cog-sync";
  // useambpres.unit = "mPa";

  // altoffset.device_type = "number";
  // altoffset.control_name = "altitude_offset";
  // altoffset.custom_settings = "\"min\": -450, \"max\": 8850, \"step\": 5, \"initial\": 0";
  // altoffset.icon = "mdi:image-filter-hdr";
  // altoffset.unit = "meters";

  // co2ref.device_type = "number";
  // co2ref.control_name = "co2_reference";
  // co2ref.custom_settings = "\"min\": 400, \"max\": 2000, \"step\": 1, \"initial\": 900";
  // co2ref.icon = "mdi:molecule-co2";
  // co2ref.unit = "ppm";    

  // refrate.device_type = "number";
  // refrate.control_name = "refreshrate";
  // refrate.custom_settings = "\"min\": 1, \"max\": 60, \"step\": 1";
  // refrate.icon = "mdi:refresh-circle";
  // refrate.unit = "minutes";

  std::vector<discovery_config_metadata> dcm = { tempoffset/*, useambpres, altoffset, co2ref, refrate*/ };  
  return dcm;
}

std::vector<discovery_measured_diagnostic_metadata> getAllDiscoveryMeasuredDiagnosticMessagesMetadata(){
  discovery_measured_diagnostic_metadata rssi;

  rssi.device_type = "sensor";
  rssi.device_class = "";  // battery | date | duration | timestamp | ... In some cases may be "None" - https://developers.home-assistant.io/docs/core/entity/sensor/#available-device-classes    
  //rssi.state_class = "measurement";  Accept default
  rssi.diag_attr = "wifi_rssi";
  rssi.icon = "mdi:wifi-strength-2";  // https://materialdesignicons.com/
  rssi.unit = ""; // RSSI is unitless
  
  std::vector<discovery_measured_diagnostic_metadata> dmdm = { rssi };  
  return dmdm;
}

std::vector<discovery_fact_diagnostic_metadata> getAllDiscoveryFactDiagnosticMessagesMetadata(){
  discovery_fact_diagnostic_metadata ip, mac, sht40_heater_intensity, sht40_heater_duration, sht40_precision;
  
  ip.device_type = "sensor";
  ip.diag_attr = "wifi_ip";
  ip.icon = "mdi:ip-network";  

  mac.device_type = "sensor";
  mac.diag_attr = "wifi_mac";
  mac.icon = "mdi:network-pos";  

  sht40_heater_intensity.device_type = "sensor";
  sht40_heater_intensity.diag_attr = "sht40_heater_intensity";
  sht40_heater_intensity.icon = "mdi:radiator";  

  sht40_heater_duration.device_type = "sensor";
  sht40_heater_duration.diag_attr = "sht40_heater_duration";
  sht40_heater_duration.icon = "mdi:camera-timer";

  sht40_precision.device_type = "sensor";
  sht40_precision.diag_attr = "sht40_precision";
  sht40_precision.icon = "mdi:bullseye-arrow";   

  std::vector<discovery_fact_diagnostic_metadata> dfdm = { ip, mac, sht40_heater_intensity, sht40_heater_duration, sht40_precision };  
  return dfdm;
}

/* If the buildShortDevicePayload() is used, and there are no config/controls that use the full device payload, then HA will never see it.
   So the full device payload is used for both kinds of discovery message. The short version is used for diagnostic messages because they 
   are always accompanied by sensor and/or control messages. */

// build discovery message - step 3 of 4
discovery_config getDiscoveryMessage(discovery_metadata disc_meta){
    return getDiscoveryMessage(disc_meta, DEVICE_ID, buildDevicePayload(DEVICE_NAME, getMAC(), DEVICE_MANUFACTURER, DEVICE_MODEL, DEVICE_VERSION), AVAILABILITY_TOPIC, STATE_TOPIC);
}

// build discovery config/control message - step 3 of 4
discovery_config getDiscoveryMessage(discovery_config_metadata disc_meta){  
  return getDiscoveryConfigMessage(disc_meta, DEVICE_ID, buildDevicePayload(DEVICE_NAME, getMAC(), DEVICE_MANUFACTURER, DEVICE_MODEL, DEVICE_VERSION), AVAILABILITY_TOPIC);
}

// build discovery measured diagnostic message - step 3 of 4
discovery_config getDiscoveryMessage(discovery_measured_diagnostic_metadata disc_meta){  
  return getDiscoveryMeasuredDiagnosticMessage(disc_meta, DEVICE_ID, buildShortDevicePayload(DEVICE_NAME, getMAC()), AVAILABILITY_TOPIC, DIAGNOSTIC_TOPIC);
}

// build discovery diagnostic fact message - step 3 of 4
discovery_config getDiscoveryMessage(discovery_fact_diagnostic_metadata disc_meta){  
  return getDiscoveryFactDiagnosticMessage(disc_meta, DEVICE_ID, buildShortDevicePayload(DEVICE_NAME, getMAC()), AVAILABILITY_TOPIC, DIAGNOSTIC_TOPIC);
}


/*
  temperature                         : sensor on SHT40 in celcius
  humidity                            : sensor on SHT40 in %RH

  pressure                            : sensor on DPS310 is atmospheric pressure in hectopascal (hPa)
  pressure_details.temperature        : sensor on DPS310 (pressure sensor) in celcius
  pressure_details.altitude           : sensor on DPS310 (pressure sensor), computed altitude in meters

  aqi                                 : calculation based on PM2.5 (2.5 µ𝑚 or smaller) 
  aqi_details.count_gt_03             : number of particles with diameter greater than 0.3 µ𝑚 in 0.1L of air      
  aqi_details.count_gt_05             : number of particles with diameter greater than 0.5 µ𝑚 in 0.1L of air        
  aqi_details.count_gt_10             : number of particles with diameter greater than 1.0 µ𝑚 in 0.1L of air        
  aqi_details.count_gt_25             : number of particles with diameter greater than 2.5 µ𝑚 in 0.1L of air        
  aqi_details.count_gt_50             : number of particles with diameter greater than 5.0 µ𝑚 in 0.1L of air        
  aqi_details.count_gt_100            : number of particles with diameter greater than 10.0 µ𝑚 in 0.1L of air
  aqi_details.pm1.0                   : µg/𝑚3 assumes standard particle density (CF=1)
  aqi_details.pm2.5                   : µg/𝑚3 assumes standard particle density (CF=1)
  aqi_details.pm10                    : µg/𝑚3 assumes standard particle density (CF=1)

  carbon_dioxide                      : sensor on SCD30 in ppm
  carbon_dioxide_details.temperature  : sensor on SCD30 (CO2 sensor) in celcius
  carbon_dioxide_details.humidity     : relative humidity sensor on SCD30 (CO2 sensor) in % RH
*/
void publishSensorData(){
  
  // temp & humidity
  sht40_data sht40 = readSHT40();

  // pressure & temperature
  dps310_data dps310 = readDPS310();

  // air particulates
  PM25_AQI_Data pm25aqi_data = readPM25AQI();
  uint16_t aqi = calculateAQI(pm25aqi_data.pm25_standard); // AQI based on particulate matter 2.5 µm in diameter or smaller (PM2.5) 

  // carbon dioxide
  scd30_data scd30 = readSCD30();

      std::string payload = "{\
\"temperature\": "+to_string(sht40.temperature, "%2.2f")+", \
\"humidity\": "+to_string(sht40.relative_humidity, "%2.2f")+", \
\"pressure\": "+to_string(dps310.pressure, "%5.1f")+", \
\"pressure_details\": {\"temperature\": "+to_string(dps310.temperature, "%2.2f")+", \"altitude\": "+to_string(dps310.altitude, "%4.1f")+"}, \
\"pm1\": "+to_string(pm25aqi_data.pm10_standard)+", \
\"pm25\": "+to_string(pm25aqi_data.pm25_standard)+", \
\"pm10\": "+to_string(pm25aqi_data.pm100_standard)+", \
\"aqi\": "+to_string(aqi)+", \
\"aqi_details\": {\"count_gt_03\": "+to_string(pm25aqi_data.particles_03um)+", \
\"count_gt_05\": "+to_string(pm25aqi_data.particles_05um)+", \
\"count_gt_10\": "+to_string(pm25aqi_data.particles_10um)+", \
\"count_gt_25\": "+to_string(pm25aqi_data.particles_25um)+", \
\"count_gt_50\": "+to_string(pm25aqi_data.particles_50um)+", \
\"count_gt_100\": "+to_string(pm25aqi_data.particles_100um)+"}, \
\"carbon_dioxide\": "+to_string(scd30.CO2, "%4.0f")+", \
\"carbon_dioxide_details\": {\"temperature\": "+to_string(scd30.temperature, "%2.2f")+", \"humidity\": "+to_string(scd30.relative_humidity, "%2.2f")+"}\
}";

      const char* payload_ch = payload.c_str();

      Serial.print(F("Publishing sensor readings: "));  
      Serial.println(payload_ch);
      mqttclient.publish(STATE_TOPIC.c_str(), payload_ch, NOT_RETAINED, QOS_0);  
}

void publishDiagnosticData(){

  sht40_config sht40 = readSHT40Config();

  std::string payload = "{\
\"wifi_rssi\": "+to_string(getRSSI())+", \
\"wifi_ip\": \""+getIP()+"\", \
\"wifi_mac\": \""+getMAC()+"\", \
\"sht40_heater_intensity\": \""+sht40.heater_intensity+"\", \
\"sht40_heater_duration\": "+to_string(sht40.heater_duration)+", \
\"sht40_precision\": \""+sht40.precision+"\" \
}";

  const char* payload_ch = payload.c_str();

  Serial.print(F("Publishing diagnostic readings: "));  
  Serial.println(payload_ch);

  mqttclient.publish(DIAGNOSTIC_TOPIC.c_str(), payload_ch, NOT_RETAINED, QOS_0);   
}


void messageReceived(String &topic, String &payload) {
  Serial.println("\nIncoming message: " + topic + " : " + payload);

  // Note: Do not use the mqttclient in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `mqttclient.loop()`.

  // Normally we only care about listening to the ".../refresh_rate/set" topic
  // However, early on we want to check the ".../refresh_rate/get" topic to see if a previously saved setting is available.
  // After checking, we will then unsubscribe from the ".../refresh_rate/get" topic. 
  // String set_rate_topic_str = String(SUB_RATE_TOPIC);
  // String get_rate_topic_str = String(PUB_RATE_TOPIC);  
  // if ((set_rate_topic_str.equals(topic)) || (get_rate_topic_str.equals(topic))){
  // }
}

void indicateWifiProblem(byte return_code){
  Serial.println(F("ERROR: Wifi problem!"));
}

void indicateMQTTProblem(byte return_code){
  Serial.println(F("ERROR: MQTT problem!"));
}

// populate list of topics to subscribe to
std::vector<std::string> getAllSubscriptionTopics(){  
  std::vector<std::string> topics = { }; 
  return topics;
}

// Runs until network and broker connectivity established and all subscriptions successful
// If network connectivity is lost and re-established, re-initialize the MQTT client
// If subscriptions fail, then will disconnect from broker, reconnect and try again.
void assertConnectivity(){
  bool pass = false;
  bool subscription_required = false;
  bool mqtt_connected = false;  
  do {
    do{
      if(assertNetworkConnectivity(LOCAL_ENV_WIFI_SSID, LOCAL_ENV_WIFI_PASSWORD)){ // will block until connected, waiting WIFI_ATTEMPT_COOLDOWN between attempts 
        // new connection established, ASSUME need to re-initialize MQTT client
        initMQTTClient(LOCAL_ENV_MQTT_BROKER_HOST, LOCAL_ENV_MQTT_BROKER_PORT, AVAILABILITY_TOPIC.c_str());          
        mqttclient.disconnect();  // necessary after init?       
        subscription_required = true;  
        delay(100); // yield to allow for mqtt client activity post initialization
      }         
      mqtt_connected = connectMQTTBroker(DEVICE_ID, LOCAL_ENV_MQTT_USERNAME, LOCAL_ENV_MQTT_PASSWORD);
      if(!mqtt_connected){
        delay(MQTT_ATTEMPT_COOLDOWN);  // yield to allow for network and mqtt client activity between broker connection attempts   
      }
    } 
    while(!mqtt_connected); // will try once to connect to broker

    // at this point has network connection and broker connection

    if(subscription_required){ // only happens when MQTT client is (re)initialized due to network disconnect
      if(!subscribeTopics(getAllSubscriptionTopics())){
        // if there is a problem with subscribing to a topic, then disconnect from the broker and try again
        indicateMQTTProblem(MQTT_SUB_ERR);
        mqttclient.disconnect();              
      }
      else{        
        pass = true;
      }
    }
    else {
      pass = true;
    }    
  }
  while(!pass);
}

void reset() {
  Serial.println(F("RESET!"));
  //initiateReset(1); // Reset.h

  // Cortex-M0+
  NVIC_SystemReset(); // core_cm0plus.h
}

bool initSensors(){
  bool sht40_ok = initSHT40();  
  Serial.println(F("************************************"));
  bool dsp310_ok = initDPS310();
  Serial.println(F("************************************"));
  bool pm25_ok = initPM25AQI();
  Serial.println(F("************************************"));
  bool scd30_ok = initSCD30();
  Serial.println(F("************************************"));

  return (sht40_ok && dsp310_ok && pm25_ok && scd30_ok);
}

void setup()
{
  #ifndef DISABLE_SERIAL_OUTPUT
  Serial.begin(9600);
  while (!Serial)
    delay(10);     // will pause Zero, Leonardo, etc until serial console opens   
  #endif

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_OFF); // initialize to off

  Serial.println(DEVICE_NAME);
  Serial.println(F("************************************"));

  if(!initSensors()){
    // one or more sensors failed to initialize
    Serial.println(F("Failed to initialize all sensors!"));
    delay(RESET_DELAY);
    reset();
  }

  // Connect to wifi & mqtt & subscribe
  if(initWifi()){
    assertConnectivity();  // Runs until network and broker connectivity established and all subscriptions successful
    printNetworkDetails();
  }
  else{
    // Wifi cannot be initialized
    Serial.println(F("Failed to initialize wifi!"));
    delay(RESET_DELAY);
    reset();
  }

  Serial.println(F("************************************"));

  // This metadata is assembled into HA-compatible discovery topics and payloads
  discovery_metadata_list = getAllDiscoveryMessagesMetadata(); 
  discovery_config_metadata_list = getAllDiscoveryConfigMessagesMetadata();
  discovery_measured_diagnostic_metadata_list = getAllDiscoveryMeasuredDiagnosticMessagesMetadata();
  discovery_fact_diagnostic_metadata_list = getAllDiscoveryFactDiagnosticMessagesMetadata();
  
  // Must successfully publish all discovery messages before proceding

  int discovery_messages_pending_publication;
  do {
    discovery_messages_pending_publication = publishDiscoveryMessages(); // Create the discovery messages and publish for each topic. Update published flag upon successful publication.
  }
  while(discovery_messages_pending_publication != 0);

  // no longer need discovery metadata, so purge it from memory
  purgeDiscoveryMetadata();

  // Publish availability online message (just once after all discovery messages have been successfully published)
  publishOnline(AVAILABILITY_TOPIC.c_str());
  publishDiagnosticData();  
}

unsigned long refresh_rate = 30000; // 5 minutes; frequency of sensor updates in milliseconds
unsigned long lastMillis = 0;
void loop()
{
  mqttclient.loop(); // potential call to messageReceived()
  assertConnectivity(); // Runs until network and broker connectivity established and all subscriptions successful

  if (millis() - lastMillis > refresh_rate) {   
    lastMillis = millis();
    
    publishSensorData();

    publishOnline(AVAILABILITY_TOPIC.c_str());
    publishDiagnosticData();
  }
}
