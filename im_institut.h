/*****************************************************************************
   @file:             im_institut.h
   Created:           2022-03-26
   Last modification: 2022-05-05
   This is part of Michael_Klima.ino
   Author and (C):    Michael Hufschmidt <michael@hufschmidt-web.de>
   License:           https://creativecommons.org/licenses/by-nc-sa/3.0/de/
 * ***************************************************************************/
#ifndef IM_INSTITUT_H
  #define IM_INSTITUT_H

/******************** Boot-Log with the Serial Monitor ***********************
 *  Copy the boot messages from the Serial Monitor and paste it here!
 *  Serial Monitor, Hardware:

Michaels Raumklima Monitor
Ikea Vindriktning on Pin 14
oneWire on Pin 12
Parasite power is: OFF (3 wire)
Found Device DS18S20 / DS1820 #0, ROM-Address = 3c 0 8 3  59 6a 6 10
Found Device DS18S20 / DS1820 #1, ROM-Address = ea 0 8 3  69 83 a3 10
Found 2 DS18x00 Sensor(s) 
Switch 0 on Pin 5
Switch 1 on Pin 4
Switch 2 on Pin 0
Switch 3 on Pin 2
Configuring local access point...
Local Access Point SSID = Michael_2, local IP address = 192.168.4.1
MAC address = EA:DB:84:E1:64:3E
Connecting to external WLAN SSID NM-AP with Password 2beannounced
.............................
External WLAN SSID NM-AP connected, ext. IP address = 10.42.1.116
HTTP server started.
Attempting MQTT connection ...
MQTT Broker = 10.42.1.1
MQTT Client id = ESP8266_Michael_2

New conversion for Device 3c 0 8 3  59 6a 6 10
New conversion for Device ea 0 8 3  69 83 a3 10
Run #1, Uptime: 0 d, 0 h, 0 m, 51 s 
Feinstaub [Vindriktning]: -1 µg/m³ (Messung #1 in Run #1)
Temperatur [DS 18x20 #0]: 21.8 °C (Messung #1 in Run #1)
Temperatur [DS 18x20 #1]: 24.3 °C (Messung #1 in Run #1)
Schalter SW0: open (High)  (Messung #1 in Run #1)
Schalter SW1: open (High)  (Messung #1 in Run #1)
Publish message: hello world #1

URL auf uh2ulnmpc54: http://192.168.1.1:8080/

******* oder mit dem anderen Modul: *******

URL auf uh2ulnmpc54: http://192.168.1.1:8081/

 * ***************************************************************************/

  #define WIFI_SSID "NM-AP"
  #define WIFI_PASS "2beannounced"
  // Local Access Point Parameters, Michael Hufschmidt, 2022-01-12
  #define APSSID "Michael_2"
  // #define MQTT_BROKER "broker.mqtt-dashboard.com"
  #define MQTT_BROKER "10.42.1.1"
  #define MQTT_USER "michael"
  #define MQTT_PASS "geheim"
  // #define MY_NTP_SERVER "at.pool.ntp.org"
  #define MY_NTP_SERVER "10.42.1.1"
#endif // IM_INSTITUT_H
