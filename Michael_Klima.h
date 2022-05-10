/*****************************************************************************
   @file:             Michael_Klima.h
   Created:           2021-12-21
   Last modification: 2022-05-10
   This is part of Michael_Klima.ino
   Author and (C):    Michael Hufschmidt <michael@hufschmidt-web.de>
   License:           https://creativecommons.org/licenses/by-nc-sa/3.0/de/
 * ***************************************************************************/
#ifndef MICHAEL_KLIMA_H
  #define MICHAEL_KLIMA_H
  // ***** Functions used in "privat.h", "im_institut.h", "michael.h", ...
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

  // ***** Sensor definition
  typedef struct {
    String param;
    String value;
    String unit;
    unsigned long measurement;
    unsigned long runID;
    String topic;
    bool active;
    std::function<String(void) > sensor_read;
  } sensor_type;

  // ***** Lines below may need modification
  // Comment out for use with real Vindriktning Sensor
  // #define TEST_MODE              // Uncomment for use without Vindriktning
  // include only one of "privat.h", "im_institut.h", "michael.h", ...
  #include "privat.h"     
  #define MSG_BUFFER_SIZE (127)
  #define DHTTYPE DHT11       // define the type of sensor (DHT11 or DHT22)
  // #define DO_BLINK         // Comment out to suppress LED blinking     
  const unsigned long wlanTimeout = 60;     // timeout in seconds
  const unsigned long mqttTimeout = 30;     // timeout in seconds
  const unsigned long processInterval = 30; // process interval in seconds
  const unsigned long refreshInterval = 60; // HTML refresh in seconds
#endif // MICHAEL_KLIMA_H
