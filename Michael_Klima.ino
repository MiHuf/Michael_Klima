/*****************************************************************************
   File:              Michael_Klima.ino, Version 1.0
   Created:           2021-12-17
   Last modification: 2023-09-14
   Program size:      Sketch 409848 Bytes (39%), Global Vars 35684 Bytes (44%)
   Author and (C):    Michael Hufschmidt <michael@hufschmidt-web.de>
   Projekt Source:    https://github.com/MiHuf/Michael_Klima
   License:           https://creativecommons.org/licenses/by-nc-sa/3.0/de/
 * ***************************************************************************/
/* Michaels Raumklima-Monitor. Inspiriert durch den Artikel "IKEA Vindiktning
   hacken", siehe Make 5/2021, Seite 14 ff und hier:
   https://techtest.org/anleitung-wlan-feinstaub-und-temperatur-sensor-ikea-vindriktning-hack/
   
   Im Original: VINDRIKTNING Ikea Luftguete Sensor -> AdafruitIO
   Siehe https://github.com/MiHuf/Michael_Klima#readme

 * ***************************************************************************/

// ***** Includes
// include board https://arduino.esp8266.com/stable/package_esp8266com_index.json
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <time.h>
#include <TZ.h>
// https://github.com/knolleary/pubsubclient
#include <PubSubClient.h>
#include <CertStoreBearSSL.h>
#include <functional>
// ***** For BME280 and SCD-30
// https://www.az-delivery.de/products/gy-bme280 (BME280)
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
// https://www.adafruit.com/product/4867  (SCD-30)
// https://learn.adafruit.com/adafruit-scd30/arduino
#include <Adafruit_SCD30.h>
// ***** External definitions, constants and variables
#include "Michael_Klima.h"

// ***** Pin definitions
#ifdef LED_BUILTIN     // true for D1 Mini, =D4
#define AN LOW
#define AUS HIGH
#else                  // not a D1 Mini
#define LED_BUILTIN 1  // LED for Digistump
#define AN HIGH
#define AUS LOW
#endif
// Original-Version aus Make 05/21:
// constexpr static const uint8_t PIN_UART_RX = D2; // =GPIO4 am Wemos D1 Mini
// Version fuer mein Adapter-Board:
constexpr static const uint8_t PIN_UART_RX = D5;   // =GPIO14 am Wemos D1 Mini
constexpr static const uint8_t PIN_UART_TX = D7;   // =GPIO13 UNUNSED
constexpr static const uint8_t ONE_WIRE_BUS = D6;  // =GPIO12 am Wemos D1 Mini
// default I2C Pins defined in Aruino.h:
// constexpr static const uint8_t SCL = D1;         // Default I2C Pins
// constexpr static const uint8_t SDA = D2;         // Default I2C Pins
constexpr static const uint8_t SW0 = D7;   // =GPIO13 / D7
// ***** Warning! Boot fails if D3 or D4 are pulled LOW !!!
constexpr static const uint8_t SW1 = D3;   // =GPIO0, *** Warning !
constexpr static const uint8_t SW2 = D3;   // =GPIO0, *** Warning !
constexpr static const uint8_t SW3 = D4;   // =GPIO2 =LED_BUILTIN ***
constexpr static const uint8_t ADC0 = A0;  // = Analog input, Pin 17

// ***** General Settings
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
const String wotag[] = { "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa" };
const uint8_t BLINK_COUNT = 5;  // Default Anzahl Lichtblitze
const uint16_t PUNKT = 200;     // Punktlaenge (Norm = 100 ms bei 60 BpM)
const uint16_t ABSTAND = 500;   // Punkt-Strich Abstand (Norm = 100 ms)
const uint16_t ZEICHEN = 1500;  // Buchstaben-Abstand im Wort (Norm = 3x Punkt)
//* Set these to your desired credentials. */
const char* extSsid = WIFI_SSID;
const char* extPassword = WIFI_PASS;
const char* mySsid = APSSID;
const char* myPassword = APPSK;
const char* mqtt_server = MQTT_BROKER;
const char* mqtt_user = MQTT_USER;
const char* mqtt_password = MQTT_PASS;
const String mq_client = "ESP8266_" + String(APSSID);
#define MQTT_CLIENT mq_client.c_str();
#ifdef MQTT_CLIENT
  const char* mqtt_client_id = MQTT_CLIENT;
#else
  // Create a random client ID
  String client_id = "ESP8266Client-" + String(random(0xffff), HEX);
  const char *mqtt_client_id = client_id.c_str()
#endif


// ***** Variables
String localIP_s, macAddress_s, externalIP_s;
bool wlanOK = false;
bool mqttOK = false;
bool timeOK = false;
time_t now;  // this is the epoch
tm tm;       // time information in a structure
#ifdef MY_TITLE
  String title = MY_TITLE;
#else
  String title = "Michaels Raumklima Monitor";
#endif
int sensorCount = sizeof(sensor) / sizeof(sensor_type);
uint8_t deviceCount = 0;  // Anzahl der OneWire - Clients
uint8_t dallasCount = 0;  // Anzahl der DS18x10 - Sensoren
// DallasTemperature.h, line 63: typedef uint8_t DeviceAddress[8];
DeviceAddress tempAddr, ds_0, ds_1;  // DS 18x20 Sensoren
uint8_t serialRxBuf[80];
uint8_t rxBufIdx = 0;
unsigned long runID = 0;
unsigned long previousMillis = 0;
String clientId = "ESP8266Client-";
char msg[MSG_BUFFER_SIZE];
String startTime = "";
double rpd = RPD;     // LDR Pull-Down Resistor
double r10 = R10;     // LDR R(10 Lux)
double sens = GAMMA;  // LDR Gamma-Value / Sensitivity


// ***** Objects
SoftwareSerial sensorSerial(PIN_UART_RX, PIN_UART_TX);
ESP8266WebServer webServer(80);
// WiFiClient espClient;
WiFiClientSecure espClient;
PubSubClient client(espClient);
// BearSSL::CertStore certStore;
OneWire oneWire(ONE_WIRE_BUS);
DHT dht(ONE_WIRE_BUS, DHTTYPE, 6);
DallasTemperature ds(&oneWire);
Adafruit_BME280 bme;   // I2C Communication
Adafruit_SCD30 scd30;  //


// ***** General Functions
void blink(const uint8_t count = BLINK_COUNT) {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, AUS);
  delay(ZEICHEN);
  for (uint8_t i = 0; i < count; i++) {
    //    Serial.printf("\nBlink #%d", i);
    digitalWrite(LED_BUILTIN, AN);
    delay(ABSTAND);
    digitalWrite(LED_BUILTIN, AUS);
    delay(ABSTAND);
  }
  delay(ZEICHEN);
  pinMode(LED_BUILTIN, INPUT_PULLUP);
}  // blink()

String deviceAddressToString(DeviceAddress a) {
  char buffer[32];
  snprintf(buffer, 30, "%0x %0x %0x %0x  %0x %0x %0x %0x",
           a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0]);
  return String(buffer);
}  // deviceAddressToString(...)

void connectWiFi() {
  unsigned long until = millis() + 1000 * wlanTimeout;
  if (wlanOK) return;
  // connecting to  external WLAN network
  // https://arduino-esp8266.readthedocs.io/en/3.0.2/esp8266wifi/readme.html
  Serial.printf("Connecting to external WLAN SSID %s with Password %s\n",
                extSsid, extPassword);
  WiFi.mode(WIFI_AP_STA);  // this is the default
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

String lz(int num) {
  // add leading zero
  if (num >= 10) {
    return String(num);
  } else {
    return "0" + String(num);
  }
}  // lz()

String getTime() {
  String out = "";
  if (timeOK) {
    time(&now);              // read the current time
    localtime_r(&now, &tm);  // update the structure tm with the current time
    out += wotag[tm.tm_wday] + ", den ";
    out += lz(tm.tm_mday) + "."
           + lz((tm.tm_mon + 1)) + "."
           + String((tm.tm_year + 1900));
    out += " ";
    out += lz(tm.tm_hour) + ":"
           + lz(tm.tm_min) + ":"
           + lz(tm.tm_sec);
  }  // if
  return out;
}  // getTime()

String runInfo() {
  unsigned long currentMillis, d, h, m, s;
  currentMillis = millis();
  String info = "";
  s = currentMillis / 1000;
  m = s / 60;
  h = m / 60;
  d = h / 24;
  info += "Run #";
  info += String(runID);
  if (timeOK) {
    info += " vom " + getTime();
  }
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
String getRandom() {
  return String(random(20, 50));
}

String getIkeaData() {  // from the Make Magazin
  int data;
  uint8_t rxBufIdx = 0;
  uint8_t checksum = 0;
  // Sensor Serial aushorchen
  while ((sensorSerial.available() && rxBufIdx < 127) || rxBufIdx < 20) {
    serialRxBuf[rxBufIdx++] = sensorSerial.read();
    // Wenn es Probleme mit dem Empfang gibt:
    delay(15);
  }
  // Pruefsumme berechnen
  for (uint8_t i = 0; i < 20; i++) {
    checksum += serialRxBuf[i];
  }
  // Header und Pruefsumme checken
  if (serialRxBuf[0] == 0x16 && serialRxBuf[1] == 0x11 && serialRxBuf[2] == 0x0B && checksum == 0) {
    data = (serialRxBuf[5] << 8 | serialRxBuf[6]);
  } else {
    data = -1;
  }
  return String(data);
}  // getIkeaData()

String getDHTTemperature() {
  return String(dht.readTemperature(), 1);
}
String getDHTHumidity() {
  return String(dht.readHumidity(), 1);
}

String getDS1820_0() {
  if (dallasCount > 0) {
    return readDS1820Temperature(ds_0);
  } else {
    return "disconnected";
  }
}
String getDS1820_1() {
  if (dallasCount > 1) {
    return readDS1820Temperature(ds_1);
  } else {
    return "disconnected";
  }
}
String readDS1820Temperature(DeviceAddress addr) {
  bool addrOK;
  bool ok = false;
  String out = "";
  float temperature = 0.0;
  addrOK = addr[0] != 0;
  if (addrOK) {
    //  ds.setWaitForConversion(false);  // makes it async
    ds.requestTemperatures();
    //  ds.setWaitForConversion(true);
    if (!ds.isConversionComplete()) {
      delay(1000);
    }
    Serial.print("\nNew conversion for Device "
                + deviceAddressToString(addr));
    ok = ds.requestTemperaturesByAddress(addr);
  } else {
    return "disconnected";
  }  // if (addrOK)
  if (ok) {
    temperature = ds.getTempC(addr);
  } else {
    Serial.println("\nError: requestTemperaturesByAddress");
    return "disconnected";
  } // if(ok)
  if (temperature != DEVICE_DISCONNECTED_C) {  // DallasTemperature.h line 36
    return String(temperature, 1);
  } else {
    return "disconnected";
  }
}  // readDS1820Temperature

String myBME280Temperature() {
  return String(bme.readTemperature(), 1);
}  // myBME280Temperature
String myBME280Humidity() {
  return String(bme.readHumidity(), 0);
}  // myBME280Humidity
String myBME280Pressure() {
  return String(bme.readPressure() / 100.0F, 1);
}  // getBME280Pressure

String mySCD30Temperature() {
  return String(scd30.temperature, 1);
}  // mySCD30Temperature
String mySCD30Humidity() {
  return String(scd30.relative_humidity, 0);
}  // mySCD30Humidity
String mySCD30CO2() {
  return String(scd30.CO2, 3);
}  // mySCD30CO2

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
}
String readSwitch(uint8_t pin) {
  String out;
  if (digitalRead(pin)) {
    out = "open (High)";
  } else {
    out = "closed (Low = GND)";
  }
  return out;
}  // readSwitch

String getADC0() {
  return String(analogRead(ADC0));
}  // getADC0()
String getLDR() {
  int adc = analogRead(ADC0);
  double b = 0.0;
  String out = "", bs = "";
  if (adc >= 1 && adc <= 1023) {
    b = 10.0 * exp(-log((rpd / r10) * (1024.0 / adc - 1.0)) / sens);
    bs = b < 100.0 ? String(b, 2) : String(b, 0);
  } else {
    bs = "overflow";
  }
  out = "ADC = " + String(adc) + " » = " + bs;  // », →, ≡ oder ⇔
  return out;
}  // getLDR()

void getSensorData() {
  String value = "";
  runID += 1;
  for (uint8_t i = 0; i < sensorCount; i++) {
    value = sensor[i].sensor_read();
    //    if (sensor[i].active) {
    sensor[i].value = value;
    if (value != "disconnected" && value != "nan" && value != "???" && value != "inf") {
      sensor[i].measurement += 1;
      sensor[i].runID = runID;
    }  // if value
       //    }  // if active
  }    // for
}  // getSensorData()

String formatSensorData(bool html = false) {
  String out = "";
  out += html ? "<p>" : "\n";
  out += String(APSSID) + " gestartet" + startTime;
  out += html ? "</p>\r\n<p>" : "\n";
  out += runInfo();
  out += html ? "</p>\r\n" : "\n";
  for (uint8_t i = 0; i < sensorCount; i++) {
    if (sensor[i].active) {
      out += html ? "<p>" : "";
      out += sensor[i].param + ": " + sensor[i].value + " " + sensor[i].unit;
      out += " (Messung #" + String(sensor[i].measurement);
      out += " in Run #" + String(sensor[i].runID) + ")";
      out += html ? "</p> \r\n" : "\n";
    }  // if active
  }    // for i
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
  page += " content=\"width=device-width, initial-scale=1.0,";
  page += " user-scalable=no\">\r\n";
  page += "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"> \r\n";
  page += "<meta name=\"language\" content=\"de\"> \r\n";
  page += "<meta http-equiv=\"refresh\" content=\"";
  page += String(refreshInterval) + "\">\r\n";
  page += "<style type=\"text/css\">\r\n";
  page += "  html {font-family: Helvetica; display: inline-block; ";
  page += "margin: 0px auto; text-align: left;}\r\n";
  page += "  body {background-color: #FFC; margin-top: 50px; margin-left: 20px;}\r\n";
  page += "  h1 {color: #33C; margin: 20px auto 30px;}\r\n";
  page += "  p {font-size: 24px; color: #933; margin-bottom: 10px;}\r\n";
  page += "</style>\r\n";
  page += "</head>\r\n";
  page += "<body>\r\n";
  page += "<div id=\"webpage\">\r\n";
  page += "<h1>" + title + "</h1> \r\n";
  page += "<p>Siehe <a href=\"https://github.com/MiHuf/Michael_Klima\" \
          target=\"_blank\">github.com/MiHuf/Michael_Klima</a></p>\r\n";
  page += "<p>Local Access Point SSID = " + String(mySsid) + ", IP address = " + localIP_s + ", MAC address = " + macAddress_s + "</p> \r\n";
  page += "<p>External WLAN SSID = " + String(extSsid);
  if (wlanOK) {
    page += ", IP address = " + externalIP_s + "</p> \r\n";
  } else {
    page += ", Timeout after " + String(wlanTimeout) + " s</p> \r\n";
  }
  if (mqttOK) {
    page += "<p>MQTT Broker = " + String(mqtt_server) + "<br>MQTT User = " + String(mqtt_user) + "<br>MQTT Client id = " + String(mqtt_client_id) + "</p> \r\n";
  } else {
    page += "<p> MQTT Timeout after " + String(mqttTimeout) + " s</p> \r\n";
  }
  page += formatSensorData(true);
  page += "</div>\r\n";
  page += "</body>\r\n";
  page += "</html>\r\n";
  return page;
}  //  buildHtml()


// ***** MQTT Functions

void reconnect() {
  unsigned long until = millis() + 1000 * mqttTimeout;
  mqttOK = client.connected();
  if (mqttOK) return;
  // Loop until we're reconnected
  while (!mqttOK && (millis() < until)) {
    Serial.println("Attempting MQTT connection ...");
    // Attempt to connect
    if (mqtt_client_id, mqtt_user, mqtt_password) {
      Serial.println("MQTT Broker = " + String(mqtt_server));
      Serial.println("MQTT User = " + String(mqtt_user));
      Serial.println("MQTT Client id = " + String(mqtt_client_id));
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
      mqttOK = true;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }  // while
  if (!mqttOK) Serial.printf("MQTT Timeout after %d s", mqttTimeout);
}  // reconnect()

void publishSensorData() {
  snprintf(msg, MSG_BUFFER_SIZE, "hello world #%ld", runID);
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish("outTopic", msg);
  //  for (uint8_t i = 0; i < sensorCount; i++) {
  //    if (sensor[i].topic.length() > 0) {
  //      ;  // **** TODO: Publish via MQTT
  //    }  // if
  //  }  // for
}  // publishSensorData()

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(LED_BUILTIN, LOW);  // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}  // callback(..)


// ***** Main Functions

void setup() {  // setup code, to run once
  bool retry;
  int connect_tries;
  String type;
  // Debug Serial
  Serial.begin(115200);
  while (!Serial)
    ;  // wait for Debug Serial
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println();
  Serial.println(title);
  Serial.printf("Defined %d sensors in the Software\n", sensorCount);
  // Software Serial für IKEA Sensor
  Serial.printf("Software UART (Ikea) on Pin %d\n", PIN_UART_RX);
  sensorSerial.begin(9600);
  pinMode(ONE_WIRE_BUS, INPUT);
  oneWire.begin(ONE_WIRE_BUS);
  if (oneWire.reset()) {
    Serial.printf("oneWire Bus on Pin %d\n", ONE_WIRE_BUS);
  } else {
    Serial.printf("No devices found on Pin %d\n", ONE_WIRE_BUS);
  }
  oneWire.begin(ONE_WIRE_BUS);
  dht.begin();
  ds.begin();
  Serial.print("Parasite power is: ");
  if (ds.isParasitePowerMode())
    Serial.println("ON (2 wire)");
  else Serial.println("OFF (3 wire)");
  //  ds.setResolution(9);            // global resulution, default 9
  if (ds.getAddress(ds_0, 0)) {
    type = ds_0[0] == DS18B20MODEL ? "DS18B20" : "DS18S20 / DS1820";
    Serial.println("Found Device " + type + " #0, ROM-Address = "
                   + deviceAddressToString(ds_0));
    dallasCount += 1;
    // ds.setResolution(ds_0, 11);     // resolutuion for this sensor
  }  // ds.getAddress(...)
  if (ds.getAddress(ds_1, 1)) {
    type = ds_0[0] == DS18B20MODEL ? "DS18B20" : "DS18S20 / DS1820";
    Serial.println("Found Device " + type + " #1, ROM-Address = "
                   + deviceAddressToString(ds_1));
    dallasCount += 1;
    // ds.setResolution(ds_1, 11);     // resolutuion for this sensor
  }  // ds.getAddress(...)
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
  pinMode(SW0, INPUT_PULLUP);  // no external pullup needed
  pinMode(SW1, INPUT_PULLUP);  // no external pullup needed
  pinMode(SW2, INPUT_PULLUP);  // no external pullup needed
  pinMode(SW3, INPUT_PULLUP);  // no external pullup needed
  Serial.printf("Switch 0 on Pin %d\n", SW0);
  Serial.printf("Switch 1 on Pin %d\n", SW1);
  Serial.printf("Switch 2 on Pin %d\n", SW2);
  Serial.printf("Switch 3 on Pin %d\n", SW3);
  Serial.printf("ADC 0 on Pin %d\n", ADC0);
  Serial.printf("LDR-Params: Rpd = %f, R10 = %f, gamma = %f\n",
                rpd, r10, sens);

  Serial.println("Configuring local access point...");
//    if (! WiFi.softAPConfig(localIP, localGateway, localNetmask)) {
//    Serial.println("AP Config Failed");
//  }
#ifdef OPEN_WIFI
  WiFi.softAP(mySsid);  // AP will be open - or -
#else
  WiFi.softAP(mySsid, myPassword);  // AP will be password protected
#endif
#ifdef OPEN_WIFI
  Serial.printf("Local Access Point SSID = %s\n", mySsid);
#else
  Serial.printf("Local Access Point SSID = %s, Password = %s\n",
                mySsid, myPassword);
#endif
  localIP_s = WiFi.softAPIP().toString();
  Serial.printf("Local IP address = %s\n", localIP_s.c_str());
  macAddress_s = WiFi.softAPmacAddress();
  Serial.printf("MAC address = %s\n", macAddress_s.c_str());
  connectWiFi();  // connect to external WLAN
  webServer.on("/", handle_OnConnect);
  webServer.begin();
  Serial.println("HTTP server started.");
  if (wlanOK) {
    reconnect();
#ifdef HAS_INTERNET
    configTime(MY_TZ, MY_NTP_SERVER);  // Here is the IMPORTANT ONE LINER
    Serial.println("NTP-Server = " + String(MY_NTP_SERVER));
    timeOK = true;
#else
    Serial.println("No NTP-Server");
#endif
  }  // wlanOK

  // Try to initialize Communication to BME280
  retry = true;
  connect_tries = 0;
  while (retry) {
    connect_tries += 1;
    retry = connect_tries < MAX_TRIES;
    if (bme.begin(0x76)) {
      retry = false;
      Serial.println("Communication to BME280 established after " + String(connect_tries) + " tries.");
    } else {
      delay(200);
    }
  }  // End while
  if (connect_tries >= MAX_TRIES) {
    Serial.println("Could not find a valid BME280 sensor after " + String(connect_tries) + " tries.");
    Serial.println("check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x");
    Serial.println(bme.sensorID(), 16);
    Serial.println("ID of 0xFF probably means a bad address");
  }
  // End Communication to BME280
  // Try to initialize Communication to SCD30
  retry = true;
  connect_tries = 0;
  while (retry) {
    connect_tries += 1;
    retry = connect_tries < MAX_TRIES;
    if (scd30.begin()) {
      retry = false;
      Serial.println("Communication to SCD30 established after " + String(connect_tries) + " tries.");
    } else {
      delay(200);
    }
  }  // End while
  if (connect_tries >= MAX_TRIES) {
    Serial.println("Could not find a valid SCD30 sensor after " + String(connect_tries) + " tries.");
  }
  // End Communication to SCD30
  blink();  // when setup finished
}  // setup()

void loop() {  // main code, repeatedly
  unsigned long currentMillis = millis();
  randomSeed(micros());
  webServer.handleClient();
  if (currentMillis - previousMillis >= 1000 * processInterval) {
    if (startTime == "" & timeOK) startTime = ": " + getTime();
    previousMillis = currentMillis;
    getSensorData();
    printSensorData();
    if (mqttOK) {
      publishSensorData();
    }
//    if (!mqttOK) {
//      reconnect();
//    } // mqttOK
//    client.loop();
#ifdef DO_BLINK
    blink();
#endif
  }  // currentMillis < ...
}  // loop()
