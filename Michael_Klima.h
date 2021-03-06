/*****************************************************************************
   @file:             Michael_Klima.h
   Created:           2021-12-21
   Last modification: 2022-06-20
   This is part of Michael_Klima.ino
   Author and (C):    Michael Hufschmidt <michael@hufschmidt-web.de>
   License:           https://creativecommons.org/licenses/by-nc-sa/3.0/de/
 * ***************************************************************************/
#ifndef MICHAEL_KLIMA_H
  #define MICHAEL_KLIMA_H
  // ***** Functions used in 
  //       "privat.h", "im_institut.h", "_xxx.h"
  // defined in Michael_Klima.ino
  String getDummy();
  String getRandom();
  String getIkeaData();
  String getDS1820_0();
  String getDS1820_1();
  String getDHT_Temperature();
  String getDHT_Humidity();
  String getSwitch_0();
  String getSwitch_1();
  String getSwitch_2();
  String getSwitch_3();
  String getADC0();
  String getLDR();

  // ***** Sensor definition
  typedef struct {
    String param;
    String value;
    String unit;
    unsigned long measurement;
    unsigned long runID;
    String topic;
    bool active;
    std::function<String(void)> sensor_read;
  } sensor_type;

  // ***** Lines below may need modification
  // include only one of
  // "privat.h", "im_institut.h", "_michael.h", "_test.h", "_andreas.h", "_annika.h", ...
  // files "_xxx.h" will not be included in the github repository.
  #include "privat.h"     
  #define MSG_BUFFER_SIZE (127)
  #define DHTTYPE DHT11       // define the type of sensor (DHT11 or DHT22)
  // #define DO_BLINK         // Comment out to suppress LED blinking
  // Default LDR Parameters for Type = GL 5539
  #ifndef RPD
    #define RPD 4.7e3        // LDR Pull-Down Resistor
  #endif
  #ifndef R10
    #define R10 75.0e3      // LDR R(10 Lux)
  #endif
  #ifndef GAMMA
    #define GAMMA 0.8       // LDR Gamma-Value / Sensitivity 
  #endif
  const unsigned long wlanTimeout = 60;     // timeout in seconds
  const unsigned long mqttTimeout = 30;     // timeout in seconds
  const unsigned long processInterval = 30; // process interval in seconds
  const unsigned long refreshInterval = 60; // HTML refresh in seconds
#endif // MICHAEL_KLIMA_H
