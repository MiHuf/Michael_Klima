/*****************************************************************************
   @file:             privat.h
   Created:           2022-04-05
   Last modification: 2023-09-20
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
#define DHTTYPE DHT11       // define the type of sensor (DHT11 or DHT22)
sensor_type sensor[] = {
  { "Feinstaub [Vindriktning]", "0", "µg/m³", 0, 0,
    "ZuHause/Wohnzimmer/Feinstaub", true, getIkeaData },
  { "Luftfeuchte [DHT11]", "0.0", "%", 0, 0,
    "ZuHause/Wohnzimmer/Luftfeuchte", false, getDHTHumidity },
  { "Temperatur [DHT11]", "0.0", "°C", 0, 0,
    "ZuHause/Wohnzimmer/Temperatur", false, getDHTTemperature },
  { "Temperatur [DS18x20 #0]", "0.0", "°C", 0, 0,
    "ZuHause/Wohnzimmer/Temperatur_ext", false, getDS1820_0 },
  { "Temperatur [DS18x20 #1]", "0.0", "°C", 0, 0,
    "ZuHause/Wohnzimmer/Temperatur_int", false, getDS1820_1 },
  { "Temperatur [BME280]", "0.0", "°C", 0, 0,
    "ZuHause/Wohnzimmer/Temperatur", false, myBME280Temperature },
  { "Luftfeuchte [BME280]", "0.0", "%", 0, 0,
    "ZuHause/Wohnzimmer/Luftfeuchte", false, myBME280Humidity },
  { "Luftdruck [BME280]", "0.0", "hPa", 0, 0,
    "ZuHause/Wohnzimmer/Luftdruck", false, myBME280Pressure },
  { "Temperatur [SCD30]", "0.0", "°C", 0, 0,
    "ZuHause/Wohnzimmer/Temperatur", false, mySCD30Temperature },
  { "Luftfeuchte [SCD30]", "0.0", "%", 0, 0,
    "ZuHause/Wohnzimmer/Luftfeuchte", false, mySCD30Humidity },
  { "CO₂ - Gehalt [SCD30]", "0.0", "ppm", 0, 0,
    "ZuHause/Wohnzimmer/CO2", false, mySCD30CO2 },
  { "Helligkeit [LDR GL5539]", "0", "Lux", 0, 0, "", true, getLDR },
  { "Schalter SW0", "open (High)", "", 0, 0, "", true, getSwitch_0 },
  { "Schalter SW1", "open (High)", "", 0, 0, "", false, getSwitch_1 },
  { "Schalter SW2", "open (High)", "", 0, 0, "", false, getSwitch_2 },
  { "Schalter SW3", "open (High)", "", 0, 0, "", false, getSwitch_3 }
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
  // #define IS_HIVEMQ
  // #define MQTT_BROKER "192.168.0.2"
  #define MQTT_USER "myUser"
  #define MQTT_PASS "myPassword"
  #define HAS_INTERNET        // comment out if not
  #define MY_NTP_SERVER "at.pool.ntp.org"
  // LDR Parameters for Type = GL 5539:
  #define RPD 10.0e3      // LDR Pull-Down Resistor
  #define R10 75.0e3      // LDR R(10 Lux)
  #define GAMMA 0.8       // LDR Gamma-Value / Sensitivity 
  
#endif // PRIVAT_H
