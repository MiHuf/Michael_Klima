/*****************************************************************************
   File:              Michael_Klima.ino, Version 1.0
   Created:           2021-12-17
   Last modification: 2022-06-22
   Program size:      Sketch 436465 Bytes (41%), Global Vars 33924 Bytes (41%)
   Author and (C):    Michael Hufschmidt <michael@hufschmidt-web.de>
   License:           https://creativecommons.org/licenses/by-nc-sa/3.0/de/
 * ***************************************************************************/
/* Michaels Raumklima-Monitor. Inspiriert durch den Artikel "IKEA Vindiktning
   hacken", siehe Make 5/2021, Seite 14 ff und hier:
   https://techtest.org/anleitung-wlan-feinstaub-und-temperatur-sensor-ikea-vindriktning-hack/
   
   Im Original: VINDRIKTNING Ikea Luftguete Sensor -> AdafruitIO

   Siehe https://github.com/MiHuf/Michael_Klima#readme

 * ***************************************************************************/

// ***** Includes
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <time.h>
#include <TZ.h>
#include <PubSubClient.h>
#include <CertStoreBearSSL.h>
#include <functional>
// ***** External definitions, constants and variables
#include "Michael_Klima.h"

// ***** Pin definitions
#ifdef LED_BUILTIN                                  // true for D1 Mini, =D4
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
constexpr static const uint8_t SW3 = D4;            // =GPIO2 =LED_BUILTIN
// ***** Warning! Will not boot if D4 / LED_BUILTIN is LOW !!!
constexpr static const uint8_t ADC0 = A0;           // = Analog input, Pin 17

// ***** General Settings
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
const String wotag[] = {"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};
const uint8_t BLINK_COUNT = 3;  // Default Anzahl Lichtblitze
const uint16_t PUNKT = 200;     // Punktlaenge (Norm = 100 ms bei 60 BpM)
const uint16_t ABSTAND = 200;   // Punkt-Strich Abstand (Norm = 100 ms)
const uint16_t ZEICHEN = 1000;  // Buchstaben-Abstand im Wort (Norm = 3x Punkt)
//* Set these to your desired credentials. */
const char *extSsid = WIFI_SSID;
const char *extPassword = WIFI_PASS;
const char *mySsid = APSSID;
const char *myPassword = APPSK;
const char* mqtt_server = MQTT_BROKER;
const char* mqtt_user = MQTT_USER;
const char* mqtt_password = MQTT_PASS;


// ***** Variables
String localIP_s, macAddress_s, externalIP_s;
bool wlanOK = false;
bool mqttOK = false;
bool timeOK = false;
time_t now;                         // this is the epoch
tm tm;                              // time information in a structure
#ifdef MY_TITLE
  String title = MY_TITLE;
#else
  String title = "Michaels Raumklima Monitor";
#endif
int sensorCount = sizeof(sensor) / sizeof(sensor_type);
uint8_t deviceCount = 0;            // Anzahl der OneWire - Clients
uint8_t dallasCount = 0;            // Anzahl der DS18x10 - Sensoren
// DallasTemperature.h, line 63: typedef uint8_t DeviceAddress[8];
DeviceAddress tempAddr, ds_0, ds_1; // DS 18x20 Sensoren
uint8_t serialRxBuf[80];
uint8_t rxBufIdx = 0;
unsigned long runID = 0;
unsigned long previousMillis = 0;
String clientId = "ESP8266Client-";
char msg[MSG_BUFFER_SIZE];
unsigned long lastMsg = 0;
String startTime = "";
double rpd = RPD;                   // LDR Pull-Down Resistor
double r10 = R10;                   // LDR R(10 Lux)
double sens = GAMMA;                // LDR Gamma-Value / Sensitivity 


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
  unsigned long until = millis() + 1000 * wlanTimeout;
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

String lz(int num) {
  // add leading zero
  if (num >= 10) {
    return String(num);
  } else {
    return "0" + String(num);
  }
} // lz()

String getTime() {
  String out = "";
  if (timeOK) {
    time(&now);                       // read the current time
    localtime_r(&now, &tm);           // update the structure tm with the current time
    out += wotag[tm.tm_wday] + ", den ";
    out += lz(tm.tm_mday) + "."
         + lz((tm.tm_mon + 1)) +  "."
         + String((tm.tm_year + 1900));
    out += " ";
    out += lz(tm.tm_hour) + ":"
         + lz(tm.tm_min)  + ":"
         + lz(tm.tm_sec);
  } // if
  return out;
} // getTime()

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
  return String(data);
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
  if (dallasCount > 1) {
    return readDS1820Temperature(ds_1);
  } else {
    return "disconnected";
  }
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
}
String readSwitch(uint8_t pin) {
  String out;
  if (digitalRead(pin)) {
    out = "open (High)";
  } else {
    out = "closed (Low = GND)";
  }
  return out;
} // readSwitch

String getADC0(){
  return String(analogRead(ADC0));
} // getADC0()
String getLDR() {
  int adc = analogRead(ADC0);
  double b  = 0.0;
  String out = "", bs = "";
  if (adc >= 1 && adc <= 1023) {
    b = 10.0 * exp(- log((rpd / r10) * (1024.0 / adc - 1.0)) / sens);
    bs = b < 100.0 ? String(b, 2) : String(b, 0);
  } else {
    bs = "overflow";
  }
  out = "ADC = " + String(adc) + " ?? = " + bs; // ??, ???, ??? oder ???
  return out ;
} // getLDR()

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
        } // if value
//    }  // if active
  }  // for
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
    } // if active
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
  page += "<p>Local Access Point SSID = " + String(mySsid) +
          ", IP address = " + localIP_s +
          ", MAC address = " + macAddress_s + "</p> \r\n";
  page +=  "<p>External WLAN SSID = " + String(extSsid);
  if (wlanOK) {
    page += ", IP address = " + externalIP_s + "</p> \r\n";
  } else {
    page += ", Timeout after " + String(wlanTimeout) + " s</p> \r\n";
  }
  if (mqttOK) {
    page += "<p>MQTT Broker = " + String(mqtt_server) +
            "<br>MQTT Client id = " + clientId + "</p> \r\n";
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

void publishSensorData() {
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", runID);
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
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}  // callback(..)

void reconnect() { 
  unsigned long until = millis() + 1000 * mqttTimeout;
  mqttOK = client.connected();
  if (mqttOK) return;
  // Loop until we're reconnected
  while (!mqttOK && (millis() < until)) {
    Serial.println("Attempting MQTT connection ...");
    // Create a random client ID
    // clientId = "ESP8266Client-";
    // clientId += String(random(0xffff), HEX);
    clientId = "ESP8266_";
    clientId += String(mySsid);
    // Attempt to connect
    if (client.connect(clientId.c_str()), mqtt_user, mqtt_password ) {
      Serial.println("MQTT Broker = " + String(mqtt_server));
      Serial.println("MQTT Client id = " + clientId);
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
  } // while
  if (!mqttOK) Serial.printf("MQTT Timeout after %d s", mqttTimeout);
} // reconnect()


// ***** Main Functions

void setup() {                                      // setup code, to run once
  String type;
  // Debug Serial
  Serial.begin(115200);
  while (! Serial) ;                                // wait for Debug Serial
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println();
  Serial.println(title);
  Serial.printf("Defined %d sensors in the Software\n", sensorCount);
  // Software Serial f??r IKEA Sensor
  Serial.printf("Software UART (Ikea) on Pin %d\n", PIN_UART_RX);
  sensorSerial.begin(9600);
  pinMode(ONE_WIRE_BUS, INPUT);
  oneWire.begin(ONE_WIRE_BUS);
  if (oneWire.reset()) {
    Serial.printf("oneWire Bus on Pin %d\n", ONE_WIRE_BUS);
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
//  } else {
//    Serial.println("Device DS18x20 #0 not found, will be set to inactive");
//    sensor[3].active = false;
  }  // ds.getAddress(...)
  if (ds.getAddress(ds_1, 1)) {
    type = ds_0[0] == DS18B20MODEL ? "DS18B20" : "DS18S20 / DS1820"; 
    Serial.println("Found Device " + type + " #1, ROM-Address = "
                 + deviceAddressToString(ds_1));
    dallasCount += 1;
    // ds.setResolution(ds_1, 11);     // resolutuion for this sensor
//  } else {
//    Serial.println("Device DS18x20 #1 not found, will be set to inactive");
//    sensor[4].active = false;
  }  // ds.getAddress(...)
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
  Serial.printf("ADC 0 on Pin %d\n", ADC0);
  Serial.printf("LDR-Params: Rpd = %f, R10 = %f, gamma = %f\n", \
                rpd, r10, sens);
//    getSensorData();                  // not yet
//    printSensorData();                // not yet
  Serial.println("Configuring local access point...");
  //    if (! WiFi.softAPConfig(localIP, localGateway, localNetmask)) {
  //    Serial.println("AP Config Failed");
  //  }
  #ifdef OPEN_WIFI
    WiFi.softAP(mySsid);                // AP will be open - or -
  #else
    WiFi.softAP(mySsid, myPassword);    // AP will be password protected
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
  connectWiFi();                        // connect to external WLAN
  webServer.on("/", handle_OnConnect);
  webServer.begin();
  Serial.println("HTTP server started.");
  if (wlanOK) {
    reconnect();
    #ifdef HAS_INTERNET
      configTime(MY_TZ, MY_NTP_SERVER); // Here is the IMPORTANT ONE LINER
      Serial.println("NTP-Server = " + String(MY_NTP_SERVER));
      timeOK = true;
    #else
      Serial.println("No NTP-Server");
    #endif
  } // wlanOK
  blink();                             // when setup finished
}  // setup()

void loop() {                                       // main code, repeatedly
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
    blink(5);
#endif
  }  // currentMillis < ...
} // loop()
