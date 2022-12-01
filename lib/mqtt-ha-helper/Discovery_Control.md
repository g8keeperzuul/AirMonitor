# How to Use the MQTT for Home Assistant Library #
## Publishing Config/Control MQTT Discovery Messages ##

## 2D. Define the Topics ##

All sensor updates are published in a single complex json payload to a single topic, so these same topics are used by all sensors or diagnostics.
The following **build*Topic()** methods are already supplied.
```
// Last will and testament topic
const std::string AVAILABILITY_TOPIC = buildAvailabilityTopic("sensor", std::string(DEVICE_ID)); // homeassistant/sensor/featherm0/availability

const std::string STATE_TOPIC = buildStateTopic("sensor", std::string(DEVICE_ID)); // homeassistant/sensor/featherm0/state
```

Controls/Configuration Discovery is a special case that requires a dedicated topic for getting and setting the control.

homeassistant/{device_type}/{device_id}/{control_name}/set
homeassistant/{device_type}/{device_id}/{control_name}/get

The device_type and control_name are defined in the next step (3D). The device_id was defined in step 1.

## 3D. Define the Control(s) ##

Define a global variable to keep track of it all:
```
std::vector<discovery_config_metadata> discovery_config_metadata_list; 
```

Add one **discovery_config_metadata** instance for each sensor you want to announce to Home Assistant and return the collection via:
```
std::vector<discovery_config_metadata> getAllDiscoveryConfigMessagesMetadata()
```

Attribute | Decription
--|--
device_type | Entity such as number, switch, light, sensor...  [See Home Assistant docs](https://developers.home-assistant.io/docs/core/entity)
control_name | Internal label for control (no spaces)
custom_settings | JSON snippet with escaped quotes - contents depend on device_type - ie. "\"min\": 1, \"max\": 10000"
icon | From [MaterialDesign](https://materialdesignicons.com/) reference
unit | Any unit

Example:
```
  tempoffset.device_type = "number";
  tempoffset.control_name = "temperature_offset";
  tempoffset.custom_settings = "\"min\": 0, \"max\": 10000, \"step\": 10, \"initial\": 0";
  tempoffset.icon = "mdi:home-thermometer";
  tempoffset.unit = "hundredths °C";
```

## 4D. Provide All Necessary Details to Build a Config/Control Discovery Message ##

Implement **getDiscoveryMessage** that takes the **discovery_config_metadata** you just created plus some additional details and returns a **discovery_config** structure. All these methods are already defined and you already defined the device details and topics, so *all you need to do is cut-and-paste this into your code*.
```
discovery_config getDiscoveryMessage(discovery_config_metadata disc_meta){
    return getDiscoveryConfigMessage( disc_meta, 
                                      DEVICE_ID, 
                                      buildDevicePayload( DEVICE_NAME, 
                                                          getMAC(), 
                                                          DEVICE_MANUFACTURER, 
                                                          DEVICE_MODEL, 
                                                          DEVICE_VERSION), 
                                      AVAILABILITY_TOPIC);                                 
}
```
## 6D. Return ??? Values ##

As per step 3, format your JSON payload like:
The payload is expected to be modelled like this:
```
{
    "<device_class>": value,
    "<device_class>_details:{ <collection of any key-value pairs> }
}
```
The *_details attribute is only necessary if has_sub_attr = true

and publish it to the **STATE_TOPIC** using:
```
void publish(String &topic, String &payload)
```


---
```
const std::string SET_<control_name>_TOPIC = buildSetterTopic(<device_type>, std::string(DEVICE_ID), <control_name>);

const std::string GET_<control_name>_TOPIC = buildGetterTopic(<device_type>, std::string(DEVICE_ID), <control_name>"); 
```

For each control, the *setter topic* must be subscribed to so that updates to the device can be sent via MQTT.
```
std::vector<std::string> getAllSubscriptionTopics()
```

A handler is needed for each control that can be updated via a topic.

Within the generic subscription handler:
```
void messageReceived(String &topic, String &payload)
```
test the topic and invoke a specialized handler with the payload
```
            String temp_offset_topic_str = String(SUB_TEMPOFFSET_TOPIC);
            if(temp_offset_topic_str.equals(topic)){
              co2_temp_offset = payload.toInt(); 
              set_temp_offset = true;
              if (co2_temp_offset < 0) { 
                co2_temp_offset = 0;        
              } 
              Serial.print(F("CO2 temperature offset updated to (one-hundredths of a degree celcius): ")); Serial.println(co2_temp_offset);
            }
```

Once the update has been processed, to reflect the change back to Home Assistant, publish it to the *getter topic*.
```
void publish<control_name>(){    
    publishConfigValue(GET_<control_name>_TOPIC, <control_value>); 
}
```