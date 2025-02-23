# Physical Build #

This device is based on the ATSAMD21G18 ARM Cortex M0 processor paired with an Atmel ATWINC1500 wifi module. Since the device is meant to run off of mains power, the power savings and sleep features are irrelevant. More importantly, this chip has  32K of RAM and 256K of FLASH - most of which is needed by the custom software and all the necessary library dependencies for the various sensors (FLASH is ~95% utilized). 

The first enclosure used a simple project box with a nice, quiet Noctua fan. The fan is needed for two reasons: 1) it draws in air to that the particulate counter gets an accurate reading and 2) it moves the warm air out to ensure an accurate temperature and humidity reading. 

![First enclosure](AirMonitor-oblique.jpg)
![Custom 3D enclosure](../content/3d-model/AirMonitor-enclosure-v2.png)

The custom designed case was inspired by [Apple's trashcan](https://duckduckgo.com/?q=apple+trashcan&iax=images&ia=images). Air is drawn in with the same Noctua fan at the top and it escapes via holes at the base.

The circuit boards remain mounted on cardboard as with the version 1 design, but this time both the AirMonitor board and RadiationWatcher boards are mounted within the cylinder.

The build was drastically simplified by using more expensive variants of the sensors that included i2c interfaces. Therefore the build is much like LEGOs, simply daisy chain a number of i2c components together. 

![Components](AirMonitor-inside.jpg)
From left to right: PM2.5 air quality sensor, SHT40 temperature and humidity, DPS310 pressure, Feather M0, and SCD-30 CO<sup>2</sup> sensor. Under the Feather M0 is a i2c daughterboard.

![i2c Adapter](sparkfun_thingplus.png)

The I2C adapter daughterboard hiding under the Feather M0.

## Hardware Parts List ##

Component | Detail | Interface | Reference
---|---|---|---
Microcontroller | Adafruit Feather M0 with ATWINC1500 wireless module | n/a | https://www.adafruit.com/product/3010
I2C Adapter | Sparkfun Thing plus | Feather daughterboard | https://www.sparkfun.com/products/16790
Barometric Pressure sensor | Adafruit DPS310 | i2c | https://www.adafruit.com/product/4494
Temperature and Humidity sensors | Adafruit Sensiron SHT40 | i2c | https://www.adafruit.com/product/4885
PM 2.5 Air Particulate sensor | Adafruit PMSA003I | i2c | https://www.adafruit.com/product/4632
Carbon Dioxide sensor | Adafruit SCD-30 - NDIR CO2 | i2c | https://www.adafruit.com/product/4867
Fan | Noctua NF-A8 5V, Premium Quiet Fan with USB Power Adaptor Cable, 3-Pin, 5V Version (80mm, Brown). 10 ohm resistor added to power cable. | 5v/gnd | https://www.amazon.ca/gp/product/B07DXNT9J9


![Feather M0 pinout](FeatherM0_pinout.png)

## Enclosure ##

The final step is fitting all these components into an enclosure. One important consideration is to ensure you gave proper ventillation. Obviously the gas and air quality sensors need airflow to function correctly. Additionally mechanically exchanging the air with a fan will prevent a temperature buildup within the case which will result in sensor inaccuracies. Small fans have an high-pitched whine to them and my first attempt was a complete annoyance. I needed something a lot quieter.
![Enclosure opened](AirMonitor-both-halves.png)
I found this new Noctua 80mm fan. They are very expensive ($20 each compared to $4 each) but are perfectly quiet and have rubber isolating attachments to dampen vibrations. 

The version 2 enclosure is inspired by the old Apple Mac Pro trashcan. The circuit boards are mounted on cardboard as they were with version 1, but this time they are oriented vertically. 
There was enough space within that I mounted the [AirMonitor](https://github.com/g8keeperzuul/AirMonitor) board on one side, and the [RadiationWatcher](https://github.com/g8keeperzuul/RadiationWatcher) on the other. There is still one USB power cable coming into the enclosure which is split between the fan and two devices. 
In order to keep the fan as quiet as possible, a 10 ohm resistor was inserted into the fan power cable. It allows enough air movement while making it as silent as possible. 

![Version 2 enclosure physical](../content/3d-model/AirMonitor-enclosure-v2-collage.png)
![Version 2 enclosure 3D model](../content/3d-model/airmonitor-enclosure-model-2.png)

3D Models:
1. [AirMonitor+RadiationWatcher base](../content/3d-model/Env-Sensor-Enclosure-base.stl)
2. [AirMonitor+RadiationWatcher mount](../content/3d-model/Env-Sensor-Enclosure-mount.stl)
3.  Body?
4. [AirMonitor+RadiationWatcher fan enclosure](../content/3d-model/Env-Sensor-Enclosure-shroud.stl)
5. [AirMonitor+RadiationWatcher fan cover](../content/3d-model/Env-Sensor-Enclosure-open-fan-cover.stl)
6. [AirMonitor+RadiationWatcher domed topper](../content/3d-model/Env-Sensor-Enclosure%20fan%20cover%202.stl)



