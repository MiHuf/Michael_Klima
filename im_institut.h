/*****************************************************************************
   @file:             im_institut.h
   Created:           2022-03-26
   Last modification: 2022-03-29
   This is part of Michael_Klima.ino
   Author and (C):    Michael Hufschmidt <michael@hufschmidt-web.de>
   License:           https://creativecommons.org/licenses/by-nc-sa/3.0/de/
 * ***************************************************************************/
#ifndef IM_INSTITUT_H
  #define IM_INSTITUT_H
/******************** Boot-Log with the Serial Monitor ***********************
Serial Monitor, Hardware:
-------------------------
Michaels Raumklima Monitor
Ikea Vindriktning on Pin 14
oneWire on Pin 12
Located Device DS18x20 #0, ROM-Address = 3c 0 8 3  59 6a 6 10  (DS18S20)
Located Device DS18x20 #1, ROM-Address = ea 0 8 3  69 83 a3 10 (DS18S20)
Parasite power is: OFF
Configuring local access point...
Local Access Point SSID = Michael_2, local IP address = 192.168.4.1
MAC address = EA:DB:84:E1:64:3E
Connecting to external WLAN SSID NM-AP with Password 2beannounced
External WLAN SSID NM-AP connected, ext. IP address = 10.42.1.116
HTTP server started.
URL auf uh2ulnmpc54: http://192.168.1.1:8080/
.......
**** oder mit dem anderen Modul:
...
MAC address = C6:5B:BE:62:D9:DD
Connecting to external WLAN SSID NM-AP with Password 2beannounced
External WLAN SSID NM-AP connected, ext. IP address = 10.42.1.169
HTTP server started.
URL auf uh2ulnmpc54: http://192.168.1.1:8081/
.......
 * ***************************************************************************/

  #define WIFI_SSID "NM-AP"
  #define WIFI_PASS "2beannounced"
  #define APSSID "Michael_2"
  // #define MQTT_BROKER "broker.mqtt-dashboard.com"
  #define MQTT_BROKER "10.42.1.1"
#endif // IM_INSTITUT_H
