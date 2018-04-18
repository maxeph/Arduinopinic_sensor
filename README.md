# Arduinopinic_sensor
This Arduino is based on DHT22 and DS18B20. The Arduino will get temperatures from both sensors (and humidity for DHT22) and send them over with a 433mhz transmitter (with CRC check). This script forms part of a broader project with a relay and a django app to consult data's over the network.

##Features

- Atmega328p-based script developped with Atom;
- Libraries are included in this repositary for an easy compilation with Atom;
- Get temperature and humidity from DHT22 with Adafruit DHT Humidity & Temperature Unified Sensor Library;
- Get temperature from DS18B20 with OneWire library;
- Convert all three floats in 2-bytes ints (instead of 4 bytes for floats). This allows to make the packet to be sent via 433mhz as tiny as possible with 2 decimals precision;
- a CRC16 calculation is performed and sent with the packet to enable the receiver to validate the data.

expliquer array avec un tableau
pacauet envoyé Manchester coed
http://mchr3k.github.io/arduino-libs-manchester/



- Debugging allowed via Serial (9600 bauds);


## Breadboard




## TO DO
- Allow energy savings with sleep modes;
- Implement I2C communication with the pi to get rid of the 433mhz transmitter if the user does not need it;
- Add lcd display for visual monitoring;


## Credits

Thank you:
- Adafruit for their great library for DHTxx sensors :
	-> https://github.com/adafruit/DHT-sensor-library
- skywodd for his tutorial regarding DS18B20 :
	-> https://www.carnetdumaker.net/articles/mesurer-une-temperature-avec-un-capteur-1-wire-ds18b20-et-une-carte-arduino-genuino/
- vinmenn for his CRC16 library and relevant examples :
	-> https://github.com/vinmenn/Crc16
