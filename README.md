# Arduinopinic_sensor
This Arduino sketch is based on DHT22 and DS18B20. The Arduino will get temperatures from both sensors (and humidity for DHT22) and send them over with a 433mhz transmitter (with CRC check). This script forms part of a broader project with a relay and a django app to consult data's over the network.

https://github.com/maxeph/Arduinopinic_relay

https://github.com/maxeph/Arduinopinic_pi

https://github.com/maxeph/Arduinopinic

## Components

- Arduino Uno (Rev3)
- DHT22 Humidity and Temperature Sensor
- DS18B20 1-Wire Temperature Sensor Probe Cable
- 4.7kΩ Resistor
- 10kΩ Resistor
- 433Mhz RF Transmitter


## Features

- Atmega328p-based script developped with Atom;
- Libraries are included in this repositary for an easy compilation with Atom;
- Get temperature and humidity from DHT22 with Adafruit DHT Humidity & Temperature Unified Sensor Library;
- Get temperature from DS18B20 with OneWire library;
- Convert all three floats in 2-bytes ints (instead of 4 bytes for floats). This allows to make the packet to be sent via 433mhz as tiny as possible with 2 decimals precision;
- a CRC16 calculation is performed and sent with the packet to enable the receiver to validate the data.
- a 9-bytes array is sent via 433mhz, the structure of the packet is as follows:

| Byte 1  | Byte 2 & Byte 3 | Byte 4 & Byte 5 |  Byte 6 & Byte 6 | Byte 8 & Byte 9|
| ------------- | ------------- | ------------- | ------------- | ------------- |
| Length  | DHT22 temper | DHT22 humidity | DS18B20 temper | CRC16 check
- the packet is sent over 433mhz with the Manchester code library
- Arduino goes in sleep mode during a certain time to reduce consumption (from 55ma to 6ma)
- Debugging allowed via Serial (9600 bauds);


## Breadboard

![](https://github.com/maxeph/Arduinopinic_sensor/blob/master/Arduinopinic_sensor_bb.png)

## Credits

Thank you:
- Adafruit for their great library for DHTxx sensors :
	-> https://github.com/adafruit/DHT-sensor-library
- skywodd for his tutorial regarding DS18B20 :
	-> https://www.carnetdumaker.net/articles/mesurer-une-temperature-avec-un-capteur-1-wire-ds18b20-et-une-carte-arduino-genuino/
- vinmenn for his CRC16 library and relevant examples :
	-> https://github.com/vinmenn/Crc16
- Mchr3K for his nice tutorial regarding machester code :
		-> http://mchr3k.github.io/arduino-libs-manchester/
