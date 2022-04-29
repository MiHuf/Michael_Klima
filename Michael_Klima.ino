/*****************************************************************************
   File:              Michael_Klima.ino, Version 1.0
   Created:           2021-12-17
   Last modification: 2022-04-09
   Program size:      Sketch 317109 Bytes (30%), Global Vars 32068 Bytes (39%)
   Author and (C):    Michael Hufschmidt <michael@hufschmidt-web.de>
   License:           https://creativecommons.org/licenses/by-nc-sa/3.0/de/
 * ***************************************************************************/
/* Michaels Raumklima-Monitor. Inspiriert durch den Artikel "IKEA Vindiktning
   hacken", siehe Make 5/2021, Seite 14 ff. Es ist geplant, weitere Sensoren
   anzuschliessen.
   Im Original: VINDRIKTNING Ikea Luftguete Sensor -> AdafruitIO

   ***** Computer: Wemos D1 Mini ESP8266
   https://www.manualslib.com/manual/1800815/Wemos-D1-Mini.html
   https://www.espressif.com/sites/default/files/documentation/0a-esp8266ex_datasheet_en.pdf
   https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf

   ***** Quellen fuer die Libraries und uebernommene Code-Bloecke:
   https://blog.rolandbaer.ch/2020/03/30/wemos-d1-mini-als-temperatur-und-luftfeuchtigkeits-webserver/
   Serial Code: https://github.com/Hypfer/esp8266-vindriktning-particle-sensor
   Examples: https://github.com/wemos/D1_mini_Examples
   ESP8266:  https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/src
             https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.h
   OneWire:  https://github.com/PaulStoffregen/OneWire/blob/master/OneWire.h
             https://www.arduino.cc/reference/en/libraries/onewire/
   DS18B20:  https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/DallasTemperature.h
   DHT11:    https://cdn-learn.adafruit.com/downloads/pdf/dht.pdf
             https://github.com/adafruit/DHT-sensor-library
   MQTT:     https://github.com/knolleary/pubsubclient
   *** TODO: add MQTT functionality
 * ***************************************************************************/

// ***** Includes
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
// #include "EspMQTTClient.h"   // Not needed!
#include <PubSubClient.h>
#include <functional>
// ***** External definitions, constants and variables
#include "Michael_Klima.h"

// ***** Pin definitions
#ifdef LED_BUILTIN                                  // true for D1 Mini
  #define AN LOW
  #define AUS HIGH
#else                                               // not a D1 Mini
  #define LED_BUILTIN 1                             // LED for Digistump
  #define AN HIGH
  #define AUS LOW
#endif
// Original-Version aus Make 05/21:
// constexpr static const uint8_t PIN_UART_RX = D2; // =GPIO4 am Wemos D1 Mini
// Version fuer mein Adapter-Board:
constexpr static const uint8_t PIN_UART_RX = D5;    // =GPIO14 am Wemos D1 Mini
constexpr static const uint8_t PIN_UART_TX = D7;    // =GPIO13 UNUNSED
constexpr static const uint8_t ONE_WIRE_BUS = D6;   // =GPIO12 am Wemos D1 Mini
constexpr static const uint8_t SW0 = SCL;           // =GPIO5 / D1 / SCL
constexpr static const uint8_t SW1 = SDA;           // =GPIO4 / D2 / SDA
constexpr static const uint8_t SW2 = D3;            // =GPIO0
constexpr static const uint8_t SW3 = D4;            // =GPIO2
const uint8_t BLINK_COUNT = 3;  // Default Anzahl Lichtblitze
const uint16_t PUNKT = 200;     // Punktlaenge (Norm = 100 ms bei 60 BpM)
const uint16_t ABSTAND = 200;   // Punkt-Strich Abstand (Norm = 100 ms)
const uint16_t ZEICHEN = 1000;  // Buchstaben-Abstand im Wort (Norm = 3x Punkt)
const unsigned long processInterval = 30; // process interval in seconds
const unsigned long refreshInterval = 60; // HTML refresh interval in seconds
//* Set these to your desired credentials. */
const char *extSsid = WIFI_SSID;
const char *extPassword = WIFI_PASS;
const char *mySsid = APSSID;
const char *myPassword = APPSK;
const char* mqtt_server = MQTT_BROKER;


// ***** Variables
String localIP_s, macAddress_s, externalIP_s;
bool wlanOK = false;
#ifdef TEST_MODE
  String title = "Michaels Raumklima Monitor [Test-Mode]";
#else
  String title = "Michaels Raumklima Monitor";
#endif
#include "Michael_Sensoren.h"
int sensorCount = sizeof(sensor) / sizeof(sensor_type);
uint8_t deviceCount = 0;            // Anzahl der OneWire - Clients
uint8_t dallasCount = 0;            // Anzahl der DS18x10 - Sensoren
// DallasTemperature.h, line 63: typedef uint8_t DeviceAddress[8];
DeviceAddress tempAddr, ds_0, ds_1;     // DS 18x20 Sensoren
uint8_t serialRxBuf[80];
uint8_t rxBufIdx = 0;
unsigned long runID = 0;
unsigned long previousMillis = 0;
char msg[MSG_BUFFER_SIZE];
unsigned long lastMsg = 0;


// ***** Objects
SoftwareSerial sensorSerial(PIN_UART_RX, PIN_UART_TX);
ESP8266WebServer webServer(80);
WiFiClient espClient;
PubSubClient client(espClient);
OneWire oneWire(ONE_WIRE_BUS);
DHT dht(ONE_WIRE_BUS, DHTTYPE, 6);
DallasTemperature ds(&oneWire);


// ***** General Functions
void blink(const uint8_t count = BLINK_COUNT) {
  digitalWrite(LED_BUILTIN, AUS);
  delay(ZEICHEN);
  for (uint8_t i = 0; i < count; i++) {
    digitalWrite(LED_BUILTIN, AN);
    delay(ABSTAND);
    digitalWrite(LED_BUILTIN, AUS);
    delay(ABSTAND);
  }
  delay(ZEICHEN);
} // blink()

String deviceAddressToString(DeviceAddress a) {
  char buffer[32];
  snprintf(buffer, 30, "%0x %0x %0x %0x  %0x %0x %0x %0x",
           a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0]);
  return String(buffer);
} // deviceAddressToString(...)

void connectWiFi() {
  unsigned long now = millis();
  unsigned long until = now + 1000 * wlanTimeout;
  if (wlanOK) return;
  // connecting to  external WLAN network
  // https://arduino-esp8266.readthedocs.io/en/3.0.2/esp8266wifi/readme.html
  Serial.printf("Connecting to external WLAN SSID %s with Password %s\n",
                extSsid, extPassword);
  WiFi.mode(WIFI_AP_STA);                       // this is the default
  WiFi.begin(extSsid, extPassword);
  // check wi-fi staus until connected
  while (WiFi.status() != WL_CONNECTED && millis() < until) {
    delay(1000);
    Serial.print(".");
  }
  if (millis() < until) {
    wlanOK = true;
    externalIP_s = WiFi.localIP().toString();
    Serial.printf("\nExternal WLAN SSID %s connected, ext. IP address = %s\n",
                  extSsid, externalIP_s.c_str());
    // WiFi.printDiag(Serial);  // for Diagnosticts
  } else {
    // softAPdisconnect(wifioff);
    Serial.printf("\nExternal WLAN SSID %s, Timeout after %d s\n",
	              extSsid, wlanTimeout);
  }
  return;
}  // connectWiFi()

String runInfo() {
  unsigned long currentMillis, d, h, m, s;
  currentMillis = millis();
  s = currentMillis / 1000;
  m = s / 60;
  h = m / 60;
  d = h / 24;
  String info = "Run #";
  info += String(runID);
  info += ", Uptime: " + String(d) + " d, ";
  info += String(h % 24) + " h, ";
  info += String(m % 60) + " m, ";
  info += String(s % 60) + " s ";
  return info;
}  // runInfo()


// ***** Get Sensor Data Functions

String getDummy() {
  return "???";
}

String getIkeaData() {  // from the Make Magazin
  int data;
  uint8_t rxBufIdx = 0;
  uint8_t checksum = 0;
  String out;
  // Sensor Serial aushorchen
  while ((sensorSerial.available() && rxBufIdx < 127) ||
         rxBufIdx < 20) {
    serialRxBuf[rxBufIdx++] = sensorSerial.read();
    // Wenn es Probleme mit dem Empfang gibt:
    delay(15);
  }
  // Pruefsumme berechnen
  for (uint8_t i = 0; i < 20; i++)  {
    checksum += serialRxBuf[i];
  }
  // Header und Pruefsumme checken
  if (serialRxBuf[0] == 0x16 && serialRxBuf[1] == 0x11 &&
      serialRxBuf[2] == 0x0B && checksum == 0) {
    data = (serialRxBuf[5] << 8 | serialRxBuf[6]);
  } else {
    data = -1;
  }
  #ifdef TEST_MODE  // Michael Hufschmidt, 2022-02-23
    data = random(20, 50);
  #endif
  out = String(data);
  return out;
} // getIkeaData()

String getDHT_Temperature() {
  return String(dht.readTemperature(), 1);
}
String getDHT_Humidity() {
  return String(dht.readHumidity(), 1);
}

String getDS1820_0() {
  return readDS1820Temperature(ds_0);
}
String getDS1820_1() {
  return readDS1820Temperature(ds_1);
}
String readDS1820Temperature(DeviceAddress addr) {
  String out = "";
  float temperature = 0.0;
  bool ok1 = true, ok2 = true;
  if (ok1 && ok2) {
//    ds.setWaitForConversion(false);  // makes it async
    ds.requestTemperatures();
//    ds.setWaitForConversion(true);
  }  // if (ok1 && ok2)
  if (!ds.isConversionComplete()) {
  delay(1000);
  }  // if not complete
  if (addr == nullptr) { return "0.0";}
  Serial.print("\nNew conversion for Device "
                  + deviceAddressToString(addr));
//  ok1 = ds.isConnected(addr);  // will loose connection
//  if (ok1) {
//    temperature = ds.getTempC(addr);
//  } else {
//    Serial.println("Device not connected");
//  } // ok1
    ok2 = ds.requestTemperaturesByAddress(addr);
    if (ok2) {
      temperature = ds.getTempC(addr);
    } else {
      Serial.println("\nError: requestTemperaturesByAddress");
      return "disconnected";
    } // ok2
  if (temperature != DEVICE_DISCONNECTED_C) { // DallasTemperature.h line 36
    return String(temperature, 1);
  } else {
    return "disconnected";
  }
} // readDS1820Temperature

String getSwitch_0() {
  return readSwitch(SW0);
}
String getSwitch_1() {
  return readSwitch(SW1);
}
String getSwitch_2() {
  return readSwitch(SW2);
}
String getSwitch_3() {
  return readSwitch(SW3);
}String readSwitch(uint8_t pin) {
  String out;
  if (digitalRead(pin)) {
    out = "open (High)";
  } else {
    out = "closed (Low = GND)";
  }
  return out;
} // readSwitch

void getSensorData() {
  String value = "";
  runID += 1;
  for (uint8_t i = 0; i < sensorCount; i++) {
    value = sensor[i].sensor_read();
    if (value != "nan" && value != "???") {
      sensor[i].value = value;
      if (value != "disconnected") {
        sensor[i].measurement += 1;
        sensor[i].runID = runID;
      }
    } // if value
  }  // for
}  // getSensorData()

void publishSensorData() {
  for (uint8_t i = 0; i < sensorCount; i++) {
    if (sensor[i].topic.length() > 0) {
      ;  // **** TODO: Publish via MQTT
    }  // if
  }  // for
}  // publishSensorData()

String formatSensorData(bool html = false) {
  String out = "";
  out += html ? "<p><br>" : "\n";
  out += runInfo();
  out += html ? "</p> \r\n" : "\n";
  for (uint8_t i = 0; i < sensorCount; i++) {
    if (sensor[i].active) {
      out += html ? "<p>" : "";
      out += sensor[i].param + ": " + sensor[i].value + " " + sensor[i].unit;
      out += " (Messung #" + String(sensor[i].measurement);
      out += " in Run #" + String(sensor[i].runID) + ")";
      out += html ? "</p> \r\n" : "\n";
    } // if
  } // for i
  return out;
}  // formatSensorData()

// ***** Output Functions

void printSensorData() {
  Serial.print(formatSensorData());
}  // printSensorData()

void handleRoot() {
  /* Just a little test message.  Go to http://192.168.4.1 in a
     web browser connected to this access point to see it. */
  webServer.send(200, "text/html", "<h1>You are connected</h1>");
}  // handleRoot()

void handle_OnConnect() {
  // getSensorData();
  webServer.send(200, "text/html", buildHtml());
}  // handle_OnConnect()

void handle_NotFound() {
  webServer.send(404, "text/plain", "Not found");
}  // handle_NotFound()

String buildHtml() {
  String page = "<!DOCTYPE HTML PUBLIC ";
  page += "\"-//W3C//DTD HTML 4.01 Transitional//EN\" ";
  page += "\"http://www.w3.org/TR/html4/loose.dtd\"> \r\n";
  page += "<html>\r\n";
  page += "<head>\r\n";
  page += "<title>" + title + "</title>\r\n";
  page += "<meta name=\"viewport\"";
  page +=   " content=\"width=device-width, initial-scale=1.0,";
  page +=   " user-scalable=no\">\r\n";
  page += "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"> \r\n";
  page += "<meta name=\"language\" content=\"de\"> \r\n";
  page += "<meta http-equiv=\"refresh\" content=\"";
  page += String(refreshInterval) + "\">\r\n";
  page += "<style type=\"text/css\">\r\n";
  page += "  html {font-family: Helvetica; display: inline-block; ";
  page +=  "margin: 0px auto; text-align: left;}\r\n";
  page += "  body {background-color: #FFC;margin-top: 50px; margin-left: 20px;}\r\n";
  page += "  h1 {color: #33C; margin: 20px auto 30px;}\r\n";
  page += "  p {font-size: 24px; color: #933; margin-bottom: 10px;}\r\n";
  page += "</style>\r\n";
  page += "</head>\r\n";
  page += "<body>\r\n";
  page += "<div id=\"webpage\">\r\n";
  page += "<h1>" + title + "</h1> \r\n";
  page += "<p>Local Access Point SSID = " + String(mySsid) +
          ", IP address = " + localIP_s +
          ", MAC address = " + macAddress_s + "</p> \r\n";
  page +=  "<p>External WLAN SSID = " + String(extSsid);
  if (wlanOK) {
    page += ", IP address = " + externalIP_s + "</p> \r\n";
  } else {
    page += ", Timeout after " + String(wlanTimeout) + " s</p> \r\n";
  }
  page += formatSensorData(true);
  page += "</div>\r\n";
  page += "</body>\r\n";
  page += "</html>\r\n";
  return page;
}  //  buildHtml()


void setup() {                                      // setup code, to run once
  String type;
  // Debug Serial
  Serial.begin(115200);
  while (! Serial) ;                                // wait for Debug Serial
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println();
  #ifdef TEST_MODE  // Michael Hufschmidt, 2022-03-23
    sensor[0].param = "Feinstaub [Test-Mode]";
  #endif
  // discoverOneWireDevices();
  Serial.println(title);
  Serial.printf("Ikea Vindriktning on Pin %d\n", PIN_UART_RX);
  // Software Serial für IKEA Sensor
  sensorSerial.begin(9600);
  pinMode(ONE_WIRE_BUS, INPUT);
  oneWire.begin(ONE_WIRE_BUS);
  if (oneWire.reset()) {
    Serial.printf("oneWire on Pin %d\n", ONE_WIRE_BUS);
  } else {
    Serial.printf("No devices found on Pin %d\n", ONE_WIRE_BUS);
  }
//  **** This does not work, deviceCount & dallasCount remain zero
//  oneWire.reset_search();
//  deviceCount = 0;
//  dallasCount = 0;
//  while (oneWire.search(tempAddr, true)) {
//    Serial.println("Located Device #" + String(deviceCount) +
//                   ", ROM-Address = " + deviceAddressToString(tempAddr));
//    if (ds.validFamily(tempAddr)) {
//      if (dallasCount == 0) {
//        std::copy(std::begin(tempAddr), std::end(tempAddr), std::begin(ds_0));
//      } else {
//        std::copy(std::begin(tempAddr), std::end(tempAddr), std::begin(ds_1));
//      }
//      dallasCount += 1;
//    }
//    deviceCount += 1;
//  }  // while
//  Serial.println("Found " + String(deviceCount) + " Devices of which " +
//                 String(dallasCount) + " DS18x00 Sensors");
//  oneWire.reset();
  oneWire.begin(ONE_WIRE_BUS);
  dht.begin();
  ds.begin();
  Serial.print("Parasite power is: ");
  if (ds.isParasitePowerMode())
    Serial.println("ON (2 wire)"); else Serial.println("OFF (3 wire)");
//  ds.setResolution(9);            // global resulution, default 9
  if (ds.getAddress(ds_0, 0)) {
    type = ds_0[0] == DS18B20MODEL ? "DS18B20" : "DS18S20 / DS1820"; 
    Serial.println("Found Device " + type + " #0, ROM-Address = "
                 + deviceAddressToString(ds_0));
    dallasCount += 1;
    // ds.setResolution(ds_0, 11);     // resolutuion for this sensor
  } else {
    Serial.println("Device DS18x20 #0 not found, will be set to inactive");
    sensor[3].active = false;
  }
  if (ds.getAddress(ds_1, 1)) {
    type = ds_0[0] == DS18B20MODEL ? "DS18B20" : "DS18S20 / DS1820"; 
    Serial.println("Found Device " + type + " #1, ROM-Address = "
                 + deviceAddressToString(ds_1));
    dallasCount += 1;
    // ds.setResolution(ds_1, 11);     // resolutuion for this sensor
  } else {
    Serial.println("Device DS18x20 #1 not found, will be set to inactive");
    sensor[4].active = false;
  }
  
//  **** This does not work
//  deviceCount = ds.getDeviceCount();
//  dallasCount = ds.getDS18Count();
//  Serial.printf("Found %d OneWire Device(s) \n", deviceCount);
  Serial.printf("Found %d DS18x00 Sensor(s) \n", dallasCount);
  // Setting extension switches
  pinMode(SW0, OUTPUT);
  pinMode(SW1, OUTPUT);
  pinMode(SW2, OUTPUT);
  pinMode(SW3, OUTPUT);
  digitalWrite(SW0, HIGH);
  digitalWrite(SW1, HIGH);
  digitalWrite(SW2, HIGH);
  digitalWrite(SW3, HIGH);
  // pinMode(SW0, INPUT);               // only with external pullup
  // pinMode(SW1, INPUT);               // only with external pullup
  // pinMode(SW2, INPUT);               // only with external pullup
  // pinMode(SW3, INPUT);               // only with external pullup
  pinMode(SW0, INPUT_PULLUP);           // no external pullup needed
  pinMode(SW1, INPUT_PULLUP);           // no external pullup needed
  pinMode(SW2, INPUT_PULLUP);           // no external pullup needed
  pinMode(SW3, INPUT_PULLUP);           // no external pullup needed
  Serial.printf("Switch 0 on Pin %d\n", SW0);
  Serial.printf("Switch 1 on Pin %d\n", SW1);
  Serial.printf("Switch 2 on Pin %d\n", SW2);
  Serial.printf("Switch 3 on Pin %d\n", SW3);
//    getSensorData();                  // not yet
//    printSensorData();                // not yet
  Serial.println("Configuring local access point...");
  //    if (! WiFi.softAPConfig(localIP, localGateway, localNetmask)) {
  //    Serial.println("AP Config Failed");
  //  }
  WiFi.softAP(mySsid);                  // AP will be open - or -
  // WiFi.softAP(mySsid, myPassword);   // AP will be password protected
  localIP_s = WiFi.softAPIP().toString().c_str();
  Serial.printf("Local Access Point SSID = %s, local IP address = %s\n",
                mySsid, localIP_s.c_str());
  macAddress_s = WiFi.softAPmacAddress();
  Serial.printf("MAC address = %s\n", macAddress_s.c_str());
  connectWiFi();                        // connect to external WLAN
  webServer.on("/", handle_OnConnect);
  webServer.begin();
  Serial.println("HTTP server started.");
  blink();                             // when setup finished
}  // setup()

void loop() {                                       // main code, repeatedly
  unsigned long currentMillis = millis();
  webServer.handleClient();
  if (currentMillis - previousMillis >= 1000 * processInterval) {
    previousMillis = currentMillis;
    getSensorData();
    printSensorData();
#ifdef DO_BLINK
    blink(5);
#endif
  }
} // loop()
