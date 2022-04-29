/*****************************************************************************
   @file:             _private.h
   Created:           2021-12-21
   Last modification: 2022-02-24
   This is part of Michael_Klima.ino
   Author and (C):    Michael Hufschmidt <michael@hufschmidt-web.de>
   License:           https://creativecommons.org/licenses/by-nc-sa/3.0/de/
 * ***************************************************************************/
#ifndef PRIVATE_H
  #define PRIVATE_H
  // Uncomment only one of these two
  #define IM_INSTITUT         // oder
  
  // External WLAN / WiFi Parameters, Michael Hufschmidt, 2022-01-16
  #ifdef ZU_HAUSE
//    Serial Monitor, Hardware:
//    Michaels Raumklima Monitor
//    Ikea Vindriktning on Pin 14
//    oneWire on Pin 12
//    Located Device DS18x20 #0, ROM-Address = ea 0 8 3  69 83 a3 10
//    Located Device DS18x20 #0, ROM-Address = c5 20 32 c  4e b5 8c 28
//    Located Device DS18x20 #1, ROM-Address = 1c 20 32 c  46 8d 93 28
//    Parasite power is: OFF
//    Configuring local access point...
//    Local Access Point SSID = Michael_1, local IP address = 192.168.4.1
//    MAC address = EA:DB:84:DD:01:B5
//    Connecting to external WLAN SSID Hufschmidt: ....
//    External WLAN SSID Hufschmidt connected, ext. IP address = 192.168.0.90
//    External WLAN SSID Hufschmidt connected, ext. IP address = 192.168.0.95 [Test-Board]
//    HTTP server started.
  #endif
  #ifdef IM_INSTITUT
//    Serial Monitor, Hardware:
//    Michaels Raumklima Monitor
//    Ikea Vindriktning on Pin 14
//    oneWire on Pin 12
//    Located Device DS18x20 #0, ROM-Address = 3c 0 8 3  59 6a 6 10
//    Located Device DS18x20 #1, ROM-Address = ea 0 8 3  69 83 a3 10
//    Parasite power is: OFF
//    Configuring local access point...
//    Local Access Point SSID = Michael_2, local IP address = 192.168.4.1
//    MAC address = EA:DB:84:E1:64:3E
//    Connecting to external WLAN SSID NM-AP with Password 2beannounced
//    ....
//    External WLAN SSID NM-AP connected, ext. IP address = 10.42.1.116
//    HTTP server started.
    #define WIFI_SSID "NM-AP"
    #define WIFI_PASS "2beannounced"
  #endif
  unsigned long wlanTimeout = 60;  // timeout in seconds
  
  // Local Access Point Parameters, Michael Hufschmidt, 2022-01-12
  #ifdef IM_INSTITUT
    #define APSSID "Michael_2"
    #define MQTT_BROKER "broker.mqtt-dashboard.com"
  #endif
  #define APPSK  "geheim";
  #define MSG_BUFFER_SIZE  (50)
//  This does not work
//  IPAddress localIP = IPAddress(192, 168, 4, 1);
//  IPAddress localGateway = IPAddress(192, 168, 4, 1);
//  IPAddress localNetmask = IPAddress(255, 255, 255, 0);
  
  #define DHTTYPE DHT11    // define the type of sensor (DHT11 or DHT22)
  // #define DO_BLINK         // Comment out to suppress LED blinking    
  #define TEST_MODE        // For testing ESP8266 without Vindriktning

  // ***** variables and constants
  String title = "Michaels Raumklima Monitor [Test-Board]";
  // String title = "Michaels Raumklima Monitor";
  const unsigned long processInterval = 30; // process interval in seconds
  const unsigned long refreshInterval = 60; // HTML refresh interval in seconds

#endif // PRIVATE_H
