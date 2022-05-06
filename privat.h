/*****************************************************************************
   @file:             privat.h
   Created:           2022-04-05
   Last modification: 2022-05-06
   This is part of Michael_Klima.ino
   Author and (C):    Michael Hufschmidt <michael@hufschmidt-web.de>
   License:           https://creativecommons.org/licenses/by-nc-sa/3.0/de/
 * ***************************************************************************/
#ifndef PRIVAT_H
  #define PRIVAT_H
/******************** This is a template file, you need to adapt it **********/

/******************** Boot-Log with the Serial Monitor ***********************
 *  Copy the boot messages from the Serial Monitor and paste it here!
 *  Serial Monitor, Hardware:
 *  -------------------------
 *  
 *  
 * ***************************************************************************/

  #define WIFI_SSID "xxx"
  #define WIFI_PASS "yyy"
  // Local Access Point Parameters, Michael Hufschmidt, 2022-01-12
  #define APSSID "zzz"
  #define OPEN_WIFI            // comment out when using open WiFi with APSSK
  #define APPSK  "geheim123"   // Min. 8 characters
  #define MQTT_BROKER "broker.mqtt-dashboard.com"
  // #define MQTT_BROKER "192.168.0.2"
  #define MQTT_USER "myUser"
  #define MQTT_PASS "myPassword"
  #define MY_NTP_SERVER "at.pool.ntp.org"
#endif // PRIVAT_H
