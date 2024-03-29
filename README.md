# Michael_Klima

Michaels Raumklima-Monitor. Inspiriert durch den Artikel
"IKEA Vindiktning hacken", siehe Make 5/2021, Seite 14 ff und hier: <https://techtest.org/anleitung-wlan-feinstaub-und-temperatur-sensor-ikea-vindriktning-hack/>

Der Code ist vorbereitet für den Anschluß weiterer Sensoren.
Im Original: VINDRIKTNING Ikea Luftgüte Sensor -> AdafruitIO

Mai 2022: Absolute (aber ungenaue) Helligkeitsmessung mit einem LDR Photowiderstand. Parameter müssen in der jeweiligen Zugangsdaten-Datei xxx.h angepasst werden. Siehe LDR-Photometer.pdf.

Sept. 2023: Luftdruck-Sensor BME280 eingebaut, CO2-Sensor SCD-30 vorbereitet.

## Installation und Konfiguration

Den Projekt-Ordner "Michael_Klima" von github klonen.

Der Mircocontroller "Wemos D1 Mini ESP8266" wird programmiert mit der Arduino IDE <https://www.heise.de/ct/artikel/Arduino-IDE-installieren-und-fit-machen-fuer-ESP8266-und-ESP32-4130814.html>. Die Sprache ist im Wesentlichen C++.

Vorher müssen einige Dateien noch angepasst werden:

 1. Datei privat.h, _xxx, ..., Sektion "***** My Settings": Zugangsdaten anpassen.

 2. Datei Michael_Klima.h, ab Zeile 51 anpassen

Anschließend mit der Arduino IDE kompiliern und auf den D1 Mini hochladen.

## Life Demo

Klicke hier: <http://hufschmidt.ddns.krude.de:8086/> oder <http://hufschmidt.ddns.krude.de:8087/>

## TODO

  * Geeigneten MQTT Broker / Server auswählen und evtl. Code anpassen

## Setup März 2022:

![Foto Setup](Klima-Monitor_1.png)

Und hier die Adapter-Platine (35mm x 42mm):

![pdf Adapter Schematic](D1-Mini_Adapter_V4_sch.png "Schaltplan")

![pdf Adapter PCB](D1-Mini_Adapter_V4_brd.png "Platine")

## Quellen für den Arduino Code und für Broker
|Was     |URL|
|--------|------------------------------------------------------------------------|
|ESP8266:|<https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/>|
|Time-Server:|<https://werner.rothschopf.net/201802_arduino_esp8266_ntp.htm>|
|OneWire:|<https://github.com/PaulStoffregen/OneWire/blob/master/OneWire.h>|
|        |<https://www.arduino.cc/reference/en/libraries/onewire/>|
|DS18B20:|<https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/DallasTemperature.h>|
|DHT11:  |<https://cdn-learn.adafruit.com/downloads/pdf/dht.pdf>|
|        |<https://github.com/adafruit/DHT-sensor-library>|
|BME280: |<https://www.az-delivery.de/products/gy-bme280>|
|SCD-30: |<https://www.adafruit.com/product/4867>|
|        |<https://learn.adafruit.com/adafruit-scd30/arduino>|
|MQTT Client:|<https://github.com/knolleary/pubsubclient>|
|MQTT HiveMQ:| <https://github.com/hivemq/hivemq-mqtt-web-client>|
| |<https://www.hivemq.com/blog/full-featured-mqtt-client-browser/>|
|MQTT ioBroker:|<https://www.iobroker.net/>|
| |<https://github.com/ioBroker/ioBroker>|
|MQTT Broker Mosquitto:|<http://www.steves-internet-guide.com/install-mosquitto-linux/>|
|MQTT Broker sensatio:|<https://www.sensatio.io/>|
|MQTT Arduino Cloud:|<https://www.cloudino.io/>|
|InfluxDB:|<https://www.ionos.de/digitalguide/hosting/hosting-technik/was-ist-influxdb/>|
|Grafana: |<https://grafana.com/>|


