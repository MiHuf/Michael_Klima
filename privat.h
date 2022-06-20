/*****************************************************************************
   @file:             privat.h
   Created:           2022-04-05
   Last modification: 2022-06-20
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
  // ***** My Sensor definition
  sensor_type sensor[] = {
    #ifdef TEST_MODE
      {"Feinstaub [Test-Mode]", "0", "µg/m³", 0, 0,
       "Privat/Wohnzimmer/Feinstaub", true, getRandom},
    #else
      {"Feinstaub [Vindriktning]", "0", "µg/m³", 0, 0,
       "Privat/Wohnzimmer/Feinstaub", true, getIkeaData},
    #endif
    {"Luftfeuchte [DHT 11]", "0.0", "%", 0, 0,
     "Privat/Wohnzimmer/Luftfeuchte", true, getDHT_Humidity},
    {"Temperatur [DHT 11]", "0.0", "°C", 0, 0,
     "Privat/Wohnzimmer/Temperatur_int", true, getDHT_Temperature},
    {"Temperatur [DS 18x20 #0]", "0.0", "°C", 0, 0,
     "Privat/Wohnzimmer/Temperatur_ext", true, getDS1820_0},
    {"Temperatur [DS 18x20 #1]", "0.0", "°C", 0, 0,
     "Privat/Wohnzimmer/Temperatur_int", true, getDS1820_1},
    {"CO₂ - Gehalt [SCD 30]", "0.0", "ppm", 0, 0, "", true, getDummy},
    {"Schalter SW0", "open (High)", "", 0, 0, "", true, getSwitch_0},
    {"Schalter SW1", "open (High)", "", 0, 0, "", true, getSwitch_1},
    {"Schalter SW2", "open (High)", "", 0, 0, "", true, getSwitch_2},
    {"Schalter SW3", "open (High)", "", 0, 0, "", true, getSwitch_3},
    {"Analog Input", "0", "/ 1024", 0, 0, "", false, getADC0},
    {"Helligkeit [LDR]", "0", "Lux", 0, 0, "", true, getLDR}
  };

  // ***** My Settings
  // #define MY_TITLE "Raumklima Monitor"  // uncomment for own title
  #define WIFI_SSID "xxx"
  #define WIFI_PASS "yyy"
  // Local Access Point Parameters, Michael Hufschmidt, 2022-01-12
  #define APSSID "zzz"
  #define OPEN_WIFI            // comment out for protected WiFi with APSSK
  #define APPSK  "geheim123"   // Min. 8 characters
  #define MQTT_BROKER "broker.mqtt-dashboard.com"
  // #define MQTT_BROKER "192.168.0.2"
  #define MQTT_USER "myUser"
  #define MQTT_PASS "myPassword"
  #define HAS_INTERNET        // comment out if not
  #define MY_NTP_SERVER "at.pool.ntp.org"
  // LDR Parameters for Type = GL 5539:
  #define RPD 4.7e3       // LDR Pull-Down Resistor
  #define R10 75.0e3      // LDR R(10 Lux)
  #define GAMMA 0.8       // LDR Gamma-Value / Sensitivity 
  
#endif // PRIVAT_H
