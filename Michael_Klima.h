/*****************************************************************************
   @file:             Michael_Klima.h
   Created:           2021-12-21
   Last modification: 2022-04-27
   This is part of Michael_Klima.ino
   Author and (C):    Michael Hufschmidt <michael@hufschmidt-web.de>
   License:           https://creativecommons.org/licenses/by-nc-sa/3.0/de/
 * ***************************************************************************/
#ifndef MICHAEL_KLIMA_H
  #define MICHAEL_KLIMA_H
  // Comment out for use with real Vindriktning Sensor
  #define TEST_MODE              // Uncomment for use without Vindriktning
  // Uncomment only one of these three
  // #include "privat.h"         // entweder
  // #include "im_institut.h"    // oder 
  #include "michael.h"     
  #define MSG_BUFFER_SIZE (127)
  #define DHTTYPE DHT11       // define the type of sensor (DHT11 or DHT22)
  // #define DO_BLINK         // Comment out to suppress LED blinking     
  unsigned long wlanTimeout = 60;  // timeout in seconds
  unsigned long mqttTimeout = 30;  // timeout in seconds
#endif // MICHAEL_KLIMA_H
