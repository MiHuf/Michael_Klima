/*****************************************************************************
   @file:             _private.h
   Created:           2021-12-21
   Last modification: 2021-12-21
   This is part of Michael_Klima.ino
   Author and (C):    Michael Hufschmidt <michael@hufschmidt-web.de>
   License:           https://creativecommons.org/licenses/by-nc-sa/3.0/de/
 * ***************************************************************************/
#ifndef PRIVATE_H
  #define PRIVATE_H

  // Keys und User,  Michael Hufschmidt, 2021-11-21
  #define IO_USERNAME "huefer"
  // define IO_KEY "utgart66"
  #define IO_KEY "aio_RFaZ72L0JJQdSeg3NbpM8Z1B74QH"

  // WLAN / WiFi Parameters, Michael Hufschmidt, 2021-11-19
  #define WIFI_SSID "NM-AP"
  #define WIFI_PASS "2beannounced"
  /*
  // Name of the Adafriut IO Feed, Michael Hufschmidt, 2021-11-19
  // see https://io.adafruit.com/huefer/feeds/
  // #define ADAFRUIT_FEED "VINDRIKTNING_1"          // #1 - naked microcontroller
  // #define ADAFRUIT_FEED "VINDRIKTNING_2"          // deleted
  #define ADAFRUIT_FEED "Wohnzimmer"                 // #2
  // https://github.com/adafruit/Adafruit_IO_Arduino/blob/master/src/AdafruitIO_Feed.h
  // https://github.com/adafruit/Adafruit_IO_Arduino/blob/master/src/AdafruitIO_Feed.cpp
  */
  // For Testing ESP8266 without any Sensor
  #define TEST_MODE

  // ***** variables and constants
  const unsigned long processInterval = 10;          // processing interval in seconds

#endif // PRIVATE_H
