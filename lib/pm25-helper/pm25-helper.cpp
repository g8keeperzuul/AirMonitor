#include "pm25-helper.h"

// ------------------------------------------------------------------------------------------
// particulate matter
Adafruit_PM25AQI aqi;

bool initPM25AQI(){
    Sprint(F("Initialize PM25 AQI sensor... "));
    aqi = Adafruit_PM25AQI();
    if(aqi.begin_I2C()) {      // connect to the sensor over I2C
        // no configuration necessary
        Sprintln(F("OK"));
        return true;
    }
    else{
        Sprintln("Failed to find PM 2.5 sensor!");
        return false;
    }
}

/*
  Wildfire smoke is 0.4 to 0.7 um (first two buckets)
  https://www.visualcapitalist.com/visualizing-relative-size-of-particles/
*/
PM25_AQI_Data readPM25AQI(){

  PM25_AQI_Data data;
  if (! aqi.read(&data)) {
    Sprintln("Could not read from AQI");
    //delay(500);  // try again in a bit!
    return data;
  }
  /*
  Sprintln(F("PM2.5 AQI Sensor"));
  Sprintln(F("---------------------------------------"));
  Sprintln(F("Concentration Units (standard)"));
  Sprintln(F("---------------------------------------"));
  Sprint(F("PM 1.0: ")); Sprint(data.pm10_standard);
  Sprint(F("\t\tPM 2.5: ")); Sprint(data.pm25_standard);
  Sprint(F("\t\tPM 10: ")); Sprintln(data.pm100_standard);
  Sprintln(F("Concentration Units (environmental)"));
  Sprintln(F("---------------------------------------"));
  Sprint(F("PM 1.0: ")); Sprint(data.pm10_env);
  Sprint(F("\t\tPM 2.5: ")); Sprint(data.pm25_env);
  Sprint(F("\t\tPM 10: ")); Sprintln(data.pm100_env);
  Sprintln(F("---------------------------------------"));
  Sprint(F("Particles > 0.3um / 0.1L air:")); Sprintln(data.particles_03um);
  Sprint(F("Particles > 0.5um / 0.1L air:")); Sprintln(data.particles_05um);
  Sprint(F("Particles > 1.0um / 0.1L air:")); Sprintln(data.particles_10um);
  Sprint(F("Particles > 2.5um / 0.1L air:")); Sprintln(data.particles_25um);
  Sprint(F("Particles > 5.0um / 0.1L air:")); Sprintln(data.particles_50um);
  Sprint(F("Particles > 10 um / 0.1L air:")); Sprintln(data.particles_100um);
  Sprintln(F("---------------------------------------"));
  */
  return data;
}

uint16_t calculateAQI(uint16_t conc_i){
  uint16_t aqi_hi, aqi_low, conc_hi, conc_low;
  
  if (conc_i >= 250) {
    // hazardous
    conc_low = 250;
    conc_hi = 500; 
    aqi_low = 301;
    aqi_hi = 500;    
  }
  else if (conc_i >= 150){
    // very unhealthy
    conc_low = 150;
    conc_hi = 249; 
    aqi_low = 201;
    aqi_hi = 300; 
  }
  else if (conc_i >= 55){
    // unhealthy
    conc_low = 55;
    conc_hi = 149; 
    aqi_low = 151;
    aqi_hi = 200; 
  }  
  else if (conc_i >= 35){
    // unhealthy for sensitive groups
    conc_low = 35;
    conc_hi = 54; 
    aqi_low = 101;
    aqi_hi = 150; 
  }   
  else if (conc_i >= 12){
    // moderate
    conc_low = 12;
    conc_hi = 34; 
    aqi_low = 51;
    aqi_hi = 100; 
  }   
  else {
    // good
    conc_low = 0;
    conc_hi = 11; 
    aqi_low = 0;
    aqi_hi = 50;     
  }

  /*
  Sprint("conc_i = "); Sprintln(conc_i);
  Sprint("aqi_hi = "); Sprintln(aqi_hi);
  Sprint("aqi_low = "); Sprintln(aqi_low);
  Sprint("conc_hi = "); Sprintln(conc_hi);
  Sprint("conc_low = "); Sprintln(conc_low);
  */
  float aqi = ((aqi_hi - aqi_low)/(conc_hi - conc_low))*(conc_i - conc_low) + aqi_low;
  //Sprint("aqi = "); Sprintln(aqi);
  return (uint16_t)aqi;
}
