# Indoor Environmental Sensor #

Overview | Details
---|---
Language: | C++ 
IDE: | Platform.io (VS Code + Platform.io extension)
Microcontroller: |  Adafruit Feather M0

This custom device connects to wifi and publishes sensor readings via MQTT at the set refresh rate frequency.

This device will send Home Assistant compatible discovery messages via MQTT. It can be configured via special MQTT topics.

![Home Assistant Device](docs/ha-device-featherm0.png)

Sensor | Measures...
---|---
SHT40 | humidity (% RH), temperature (C)
DPS310 | temperature (C), pressure (hectopascal, hPa), altitude computed
PM2.5 | particulate matter (Particles > 0.3um, 0.5um, 1.0um, 2.5um, 5.0um, 10.0um / 0.1L air); **AQI computed**
SCD30 | carbon dioxide (PPM direct from sensor), humidity (% RH), temperature (C)

## How the Device Publishes Sensor State ##

MQTT Topic:
```
homeassistant/{device_type}/environmental/{device_id}/state 
homeassistant/sensor/environmental/featherm0/state
```

Sensor State Payload:
```
{
  "temperature": 22.05,
  "humidity": 38.25,
  "pressure": 989.4,
  "pressure_details": {
    "temperature": 21.25,
    "altitude": 281.3
  },
  "pm1": 1,
  "pm25": 1,
  "pm10": 1,
  "aqi": 4,
  "aqi_details": {
    "count_gt_03": 228,
    "count_gt_05": 68,
    "count_gt_10": 2,
    "count_gt_25": 0,
    "count_gt_50": 0,
    "count_gt_100": 0
  },
  "carbon_dioxide": 954,
  "carbon_dioxide_details": {
    "temperature": 23.5,
    "humidity": 35.54
  }
}
```

## How the Device Publishes Diagnostics ##

```
homeassistant/{device_type}/{device_id}/diagnostics
homeassistant/sensor/featherm0/diagnostics
```

Configuration State Payload:
```
{ 
"wifi_rssi": -42, 
"wifi_ip": "10.0.0.130", 
"wifi_mac": "96:e6:f1:05:f0:f8",
"refresh_rate": 60000, 
"dps310_computed_alt": 201.2, 
"sht40_heater_intensity": "Disabled", 
"sht40_heater_duration": 0, 
"sht40_precision": "High", 
"scd30_forced_calibration_ref": 400, 
"scd30_self_calibration_enabled": false, 
"scd30_ambient_pressure": 0, 
"scd30_altitude": 200, 
"scd30_temperature_offset": 0, 
"scd30_measurement_interval": 25 
}
```

### Altitude Offset ###

Discovery: 
```
homeassistant/number/featherm0_altitude_offset/config
```
Altitude offset state: 
```
homeassistant/sensor/featherm0/altitude_offset/get
```
Altitude offset command: 
```
homeassistant/sensor/featherm0/altitude_offset/set
```

### Ambient Pressure Calibration ###
Discovery: 
```
homeassistant/switch/featherm0_use_ambient_pressure/config
```
Use ambient pressure state: 
```
homeassistant/sensor/featherm0/use_ambient_pressure/get
```
Use ambient pressure command: 
```
homeassistant/sensor/featherm0/use_ambient_pressure/set
```

### Temperature Offset ###
Discovery: 
```
homeassistant/number/featherm0_temperature_offset/config
```
Temp Offset state: 
```
homeassistant/sensor/featherm0/temperature_offset/get
```
Temp Offset command: 
```
homeassistant/sensor/featherm0/temperature_offset/set
```

### Refresh Rate ###
Discovery: 
```
homeassistant/number/featherm0_refreshrate/config
```
Refresh rate state: 
```
homeassistant/sensor/featherm0/refreshrate/get
```
Refresh rate command: 
```
homeassistant/sensor/featherm0/refreshrate/set
```

### Environmental Sensors ###
All of the following sensors use the common sensor state topic (see below):

Sensor Discovery: 
```
homeassistant/sensor/featherm0/carbon_dioxide/config
homeassistant/sensor/featherm0/aqi/config
homeassistant/sensor/featherm0/pm1/config
homeassistant/sensor/featherm0/pm25/config
homeassistant/sensor/featherm0/pm10/config
homeassistant/sensor/featherm0/pressure/config
homeassistant/sensor/featherm0/humidity/config
homeassistant/sensor/featherm0/temperature/config
```

Controls/Config and Diagnostics Discovery:
```
homeassistant/sensor/featherm0/wifi_rssi/config
homeassistant/sensor/featherm0/wifi_ip/config
homeassistant/sensor/featherm0/wifi_mac/config
homeassistant/sensor/featherm0/sht40_heater_intensity/config
homeassistant/sensor/featherm0/sht40_heater_duration/config
homeassistant/sensor/featherm0/sht40_precision/config
```

This device cannot save any configuration parameters locally, but it does read config values from *retained* MQTT topics. 


## Software Dependencies ##

Feature | Library | Release
---|---|---
Wifi connectivity | https://github.com/arduino-libraries/WiFi101 | v0.16.1
MQTT connectivity | https://github.com/256dpi/arduino-mqtt | v2.5.0
DPS310 Barometric Pressure | https://github.com/adafruit/Adafruit_DPS310 | v1.1.1
SHT40 Temperature and Humidity | https://github.com/adafruit/Adafruit_SHT4X | v1.0.1
PM2.5 Air Quality | https://github.com/adafruit/Adafruit_PM25AQI | v1.0.6
SCD30 Carbon Dioxide | https://github.com/adafruit/Adafruit_SCD30 | v1.0.8


---
Tips:

To enable serial logging, see ha-mqtt-helper.h

To customize network configuration, copy [sample-env.h](src/sample-env.h) to env.h and apply your own local wifi and MQTT broker details.