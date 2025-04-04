/*****************************************************************************
   @file:             Michael_Klima.h
   Created:           2021-12-21
   Last modification: 2025-04-01
   This is part of Michael_Klima.ino
   Author and (C):    Michael Hufschmidt <michael@hufschmidt-web.de>
   License:           https://creativecommons.org/licenses/by-nc-sa/3.0/de/
 * ***************************************************************************/
#ifndef MICHAEL_KLIMA_H
  #define MICHAEL_KLIMA_H
  // ***** Functions used in 
  //       "privat.h", "_xxx.h"
  // defined in Michael_Klima.ino
  String getDummy();
  String getRandom();
  String getIkeaData();
  String getDS1820();
  String getDHTTemperature();
  String getDHTHumidity();
  String myBME280Temperature();
  String myBME280Humidity();
  String myBME280Pressure();
  String mySCD30Temperature();
  String mySCD30Humidity();
  String mySCD30CO2();
  String getSwitch();
  String getSwitchNum();
  String getADC0();
  String getLDR();
  String getLDRNumLog();

  // ***** Sensor definition
  typedef struct {
    String param;
    String value;
    String unit;
    unsigned long measurement;
    unsigned long runID;
    const char * topic;
    bool active;
    std::function<String(void)> sensor_read;
    uint8_t n;                     // Parameter for sensor_read
  } sensor_type;

  // ***** Lines below may need modification
  // #define DHTTYPE xxx // moved to individual header files
  // include only one of
  // "privat.h" (zzz), "_test1.h" (1), "_institut.h" (2), "_test3.h" (3), "_andreas.h" (4), "_test5.h" (5), "_wohnzimmer.h" (6), "_dach.h" (7), ...
  // (x) is the number of the controller, files "_xxx.h" will not be included in the github repository.
  #include "privat.h"
  #ifndef TOPIC
    #define TOPIC "zuHause"
  #endif
  #define MAX_DS_COUNT 10       // Max. Number of DS18x20 Sensors 
  #define MSG_BUFFER_SIZE (255)
  // Default LDR Parameters for Type = GL 5539
  #ifndef RPD
    #define RPD 10.0e3          // LDR Pull-Down Resistor
  #endif
  #ifndef R10
    #define R10 75.0e3          // LDR R(10 Lux)
  #endif
  #ifndef GAMMA
    #define GAMMA 0.8           // LDR Gamma-Value / Sensitivity 
  #endif
  #define MAX_TRIES 10          // Max. tries to connect a sensor

  #ifndef INDIVIDUAL_TIMING
  // #define DO_BLINK                         // Blink at the end of loop()
    const unsigned long wlanTimeout = 60;     // timeout in seconds
    const unsigned long mqttTimeout = 60;     // timeout in seconds
    const unsigned long processInterval = 60; // process interval in seconds
    const unsigned long refreshInterval = 60; // HTML refresh in seconds
  #endif                       // INDIVIDUAL_TIMING
#endif // MICHAEL_KLIMA_H
