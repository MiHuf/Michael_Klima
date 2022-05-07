/*****************************************************************************
   @file:             Michael_Sensoren.h
   Created:           2022-04-08
   Last modification: 2022-05-07
   This is part of Michael_Klima.ino
   Author and (C):    Michael Hufschmidt <michael@hufschmidt-web.de>
   License:           https://creativecommons.org/licenses/by-nc-sa/3.0/de/
 * ***************************************************************************/
#ifndef MICHAEL_SENSOREN_H
  #define MICHAEL_SENSOREN_H

  // ***** Functions used below, defined in Michael_Klima.ino
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
  
  // ***** Sensors should be defined here
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

  #ifdef PRIVAT_H
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
      {"Schalter SW1", "open (High)", "", 0, 0, "", true, getSwitch_1}
    };
  #endif  // PRIVAT_H

  #ifdef IM_INSTITUT_H
    sensor_type sensor[] = {
      #ifdef TEST_MODE
        {"Feinstaub [Test-Mode]", "0", "µg/m³", 0, 0,
         "Uni/Büro/Feinstaub", true, getRandom},
      #else
        {"Feinstaub [Vindriktning]", "0", "µg/m³", 0, 0,
         "Uni/Büro/Feinstaub", true, getIkeaData},
      #endif
      {"Luftfeuchte [DHT 11]", "0.0", "%", 0, 0,
       "Uni/Büro/Luftfeuchte", false, getDHT_Humidity},
      {"Temperatur [DHT 11]", "0.0", "°C", 0, 0,
       "Uni/Büro/Temperatur_int", false, getDHT_Temperature},
      {"Temperatur [DS 18x20 #0]", "0.0", "°C", 0, 0,
       "Uni/Büro/Temperatur_ext", true, getDS1820_0},
      {"Temperatur [DS 18x20 #1]", "0.0", "°C", 0, 0,
       "Uni/Büro/Temperatur_int", true, getDS1820_1},
      {"CO₂ - Gehalt [SCD 30]", "0.0", "ppm", 0, 0, "", false, getDummy},
      {"Schalter SW0", "open (High)", "", 0, 0, "", true, getSwitch_0},
      {"Schalter SW1", "open (High)", "", 0, 0, "", true, getSwitch_1}
    };
  #endif  // IM_INSTITUT_H

  #ifdef MICHAEL_H
    sensor_type sensor[] = {
      #ifdef TEST_MODE
        {"Feinstaub [Test-Mode]", "0", "µg/m³", 0, 0,
         "ZuHause/Wohnzimmer/Feinstaub", true, getRandom},
      #else
        {"Feinstaub [Vindriktning]", "0", "µg/m³", 0, 0,
         "ZuHause/Wohnzimmer/Feinstaub", true, getIkeaData},
      #endif
      {"Luftfeuchte [DHT 11]", "0.0", "%", 0, 0,
       "ZuHause/Wohnzimmer/Luftfeuchte", true, getDHT_Humidity},
      {"Temperatur [DHT 11]", "0.0", "°C", 0, 0,
       "ZuHause/Wohnzimmer/Temperatur_int", false, getDHT_Temperature},
      {"Temperatur [DS 18x20 #0]", "0.0", "°C", 0, 0,
       "ZuHause/Wohnzimmer/Temperatur_ext", true, getDS1820_0},
      {"Temperatur [DS 18x20 #1]", "0.0", "°C", 0, 0,
       "ZuHause/Wohnzimmer/Temperatur_int", true, getDS1820_1},
      {"CO₂ - Gehalt [SCD 30]", "0.0", "ppm", 0, 0, "", false, getDummy},
      {"Schalter SW0", "open (High)", "", 0, 0, "", true, getSwitch_0},
      {"Schalter SW1", "open (High)", "", 0, 0, "", true, getSwitch_1}
    };
  #endif  // MICHAEL_H
#endif // MICHAEL_SENSOREN_H
