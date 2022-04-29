/*****************************************************************************
   File:              Michael_Klima.ino, Version 1.0
   Created:           2021-12-17
   Last modification: 2021-12-23
   Program size:      Sketch 271033 Bytes (25%), Globals 28624 Bytes (34%)
   Author and (C):    Michael Hufschmidt <michael@hufschmidt-web.de>
   License:           https://creativecommons.org/licenses/by-nc-sa/3.0/de/
 * ***************************************************************************/
/* Michaels Raumklima-Monitor. Inspiriert duch den Artikel IKEA Vindiktning
   hacken, siehe Make 5/2021, Seite 14 ff. Es ist geplant, weitere Sensoren
   anzuschließen.
   Im Original: VINDRIKTNING Ikea Luftgüte Sensor -> AdafruitIO
  
   ***** Computer: Wemos D1 Mini ESP8266
   https://www.manualslib.com/manual/1800815/Wemos-D1-Mini.html
   https://www.espressif.com/sites/default/files/documentation/0a-esp8266ex_datasheet_en.pdf
   https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf

   ***** Quellen für die Libraries und übernommene Code-Blöcke:
   Serial Code: https://github.com/Hypfer/esp8266-vindriktning-particle-sensor
   Examples: https://github.com/wemos/D1_mini_Examples
   ESP8266:  https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/src
             https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.h
   OneWire:  https://github.com/PaulStoffregen/OneWire/blob/master/OneWire.h
             https://www.arduino.cc/reference/en/libraries/onewire/
   DS18B20:  https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/DallasTemperature.h
   DHT11:    https://cdn-learn.adafruit.com/downloads/pdf/dht.pdf
             https://github.com/adafruit/DHT-sensor-library
 * ***************************************************************************/

// ***** Pin definitions
#ifdef LED_BUILTIN                                  // true for D1 Mini
  #define AN LOW
  #define AUS HIGH
#else                                               // not a D1 Mini
  #define LED_BUILTIN 1                             // LED for Digistump
  #define AN HIGH
  #define AUS LOW
#endif
// #define MAKER_MAGAZIN
#ifdef  MAKER_MAGAZIN       // Original-version aus Make: 05/21
  constexpr static const uint8_t PIN_UART_RX = D2;  // =GPIO4 am Wemos D1 Mini
#else                       // Version für mein Adapter-Board V1.x
  constexpr static const uint8_t PIN_UART_RX = D5;  // =GPIO14 am Wemos D1 Mini
#endif
constexpr static const uint8_t PIN_UART_TX = D7;    // =GPIO13, UNUNSED
constexpr static const uint8_t ONE_WIRE_BUS = D6;   // =GPIO12 am Wemos D1 Mini

// ***** Includes
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

// ***** Constants (alle Zeiten in Millisekunden)
const uint8_t BLINK_COUNT = 3;  // Default Anzahl Lichtblitze
const uint16_t PUNKT = 200;     // Punktlaenge (Norm = 100 ms bei 60 BpM)
const uint16_t ABSTAND = 200;   // Punkt-Strich Abstand (Norm = 100 ms)
const uint16_t ZEICHEN = 1000;  // Buchstaben-Abstand im Wort (Norm = 3x Punkt)

// ***** Functions
void blink(const uint8_t count = BLINK_COUNT) {
  for (uint8_t i = 0; i < count; i++) {
    digitalWrite(LED_BUILTIN, AN);
    delay(ABSTAND);
    digitalWrite(LED_BUILTIN, AUS);
    delay(ABSTAND);
  }
  delay(ZEICHEN);
}

// ***** Objects
SoftwareSerial sensorSerial(PIN_UART_RX, PIN_UART_TX);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds1820(&oneWire);
DHT dht11(ONE_WIRE_BUS, DHT11);

// ***** Variables
uint8_t deviceCount = 0;     // Anzahl der OneWire - Clients
uint8_t dallasCount = 0;     // Anzahl der DS18x10 - Sensoren
float temperature;           // hier wird die Temperatur gespeichert
uint8_t serialRxBuf[80];
uint8_t rxBufIdx = 0;
unsigned long previousMillis = 0;

// ***** vaiables and constants, -> external
#include "_private.h"

void setup() {                                      // put your setup code here, to run once
  // Debug Serial
  Serial.begin(115200);
  while (! Serial);                                 // wait for Debug Serial
  delay(1000);
  Serial.print("Michaels Raumklima Monitor\n");
  pinMode(LED_BUILTIN, OUTPUT);
  if (oneWire.reset()) {
    Serial.printf("oneWire on Pin %d\n", ONE_WIRE_BUS);
  } else {
    Serial.printf("No devices found on Pin %d\n", ONE_WIRE_BUS);
  }
    blink();                                        // when setup finished
}

void loop() {                                       // put your main code here, to run repeatedly
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000 * processInterval) {
    previousMillis = currentMillis;
    // do some work
    blink(5);
  }
}
