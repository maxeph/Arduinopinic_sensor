//TODO  signature id?
//
// Version 0.02
// - Reorganisation of pin following use of standalone atmega328p
// - Sleep mode implemented decreasing consumption to ~6mAs
// - Simplifying DS18B20 reading and translating comment in english
// Version 0.01
// 1st Working version with DS18B20 qnd DHT22 sent over 433mhz on arduino uno
// consumption ~= 55mA
// https://github.com/maxeph/Arduinopinic_sensor.git

#include <Arduino.h>
#include <LowPower.h>
#include <Manchester.h>  //Initialising 433 wireless library
#include <Crc16.h>  // CRCcheck library
#include <DHT.h> // DHT library
#include <OneWire.h> // Library to communicate with DS18B20

// Declaring definitions

#define DEBUG 2// if 0 nothing; 1 just disclosing values, 2 full debug
#define TX_433 A4 // Pin connecter to Transmitter
#define DHTPIN A5 // Pin DHT
#define DHTTYPE DHT22 // type of sensor
#define DSPIN 6 // Pin DS18B20
#define NBPARAM 3 // Number of int sent
#define MSGLEN NBPARAM*2 // Msg len is 6 = 3 signed int (2 bytes each)
#define PCKTLEN MSGLEN+3 // +1 for the lenght of the msgpacket +2 for CRC 16
#define DELAY SLEEP_8S // delay between two measurments/sending min = 2000 if below the sensor heats and values will be wrong

// Declaring structs

union intarray { // shared memory for int and byte array to get its bytes
  int ints;
  byte part[2];
};

// Declaring variables

byte msgpacket[PCKTLEN] = {PCKTLEN}; // init unsigned bytes to be sent over
intarray itempext, itempeau, ihumid, crc_local;
float tempext, tempeau, humid; // floats to be used with sensors
DHT dht(DHTPIN, DHTTYPE); // Declaring sensor
int nloop = 1; // Counting n° Sending
OneWire ds(DSPIN);

// Declaring functions

void float2int(float *farg, int *intresult) { // Convert float 2 a signed int for 2 decimal precision
  *intresult = *farg * 100;
}

int bytes2int(byte arg[2]) { // Convert a 2byte array into int
  intarray result;
  result.part[0] = arg[0];
  result.part[1] = arg[1];
  return result.ints;
}

int getcrc(byte msg[PCKTLEN]) { // get 16bit CRC
  Crc16 crc; // init CRC16 object
  for (uint8_t i = 0;i<PCKTLEN-2;i++) {
    crc.updateCrc(msg[i]);
  }
  return crc.getCrc();
}

byte getTemperature(float *temperature, byte reset_search) { // Reading DS18B20
  byte data[9], addr[8];

  if (reset_search) ds.reset_search();  // Reset of bus 1-Wire if requested (to read first sensor)
  if (!ds.search(addr)) return 0xFF; //search next sensor and return error if none found
  if (OneWire::crc8(addr, 7) != addr[7]) return 0xFF; // CRC check of  address received
  if (addr[0] != 0x28) return 0xFF; // Check if DS18B20 if not -> error
  ds.reset();   // Selecting the sensor found
  ds.select(addr);
  ds.write(0x44, 1); // Requesting a temperature measure and waiting
  delay(800);

  ds.reset(); // Requesting a temperature sending
  ds.select(addr);
  ds.write(0xBE);

  // Reading the array received
  for (byte i = 0; i < 9; i++) data[i] = ds.read();

  // Converting to Celsius
  *temperature = (int16_t) ((data[1] << 8) | data[0]) * 0.0625;
  // No error
  return 0x01;
}

void buildpacket(byte msg[PCKTLEN], byte part1[2], byte part2[2], byte part3[2]) { // build array to be sent
  msg[0] = PCKTLEN;
  msg[1] = part1[0];
  msg[2] = part1[1];
  msg[3] = part2[0];
  msg[4] = part2[1];
  msg[5] = part3[0];
  msg[6] = part3[1];
  msg[PCKTLEN-2] = 0;
  msg[PCKTLEN-1] = 0;
}

void setup() {

  man.setupTransmit(TX_433, MAN_600); // Initialising 433 wireless
  dht.begin(); // Init DHT sensor

  if (DEBUG == 1 || DEBUG == 2) {  // Sending over Serial to make sure it works
    Serial.begin(9600);
    Serial.println("Initialising debug mode...");
  }

}

void loop() {
  LowPower.powerDown(DELAY, ADC_OFF, BOD_OFF);
  tempext = dht.readTemperature(); // minimum delay is 2 seconds if not the sensor heats and values are wrong
  humid = dht.readHumidity();
  if (isnan(tempext) || isnan(humid)) {
    if (DEBUG == 1 || DEBUG == 2) {
      Serial.println("Failed to read from DHT sensor!");
      dht.begin();
    }
    return;
  }

  if (getTemperature(&tempeau, true) != 0x01) {
    Serial.println("Failed to read from DS18B20 sensor!");
    return;
  }

  float2int(&tempext, &itempext.ints); // converting floats to int and storing in union objects
  float2int(&tempeau, &itempeau.ints);
  float2int(&humid, &ihumid.ints);
  buildpacket(msgpacket,itempext.part,itempeau.part,ihumid.part); // Building packet to be sent over
  crc_local.ints = getcrc(msgpacket); // Calculating CRC16
  msgpacket[PCKTLEN-2] = crc_local.part[0]; // Including raw CRC in msgpacket
  msgpacket[PCKTLEN-1] = crc_local.part[1];
  man.transmitArray(PCKTLEN, msgpacket);

  if (DEBUG == 1 || DEBUG == 2) { // Showing value received from sensors
    Serial.print("########## PACKET N° "); // Showing raw data
    Serial.print(nloop);
    Serial.println(" #################");
    Serial.print("Out temp (°C) : ");
    Serial.print(float(itempext.ints)/100);
    Serial.print(" # Water temp (°C) : ");
    Serial.print(float(itempeau.ints)/100);
    Serial.print(" # Humidity (%) : ");
    Serial.println(float(ihumid.ints)/100);

    if (DEBUG == 2) { // Showing raw data sent over
      Serial.print("PCK LEN: ");
      Serial.print(msgpacket[0],HEX);
      for(uint8_t i=1;i<PCKTLEN-2;i++) {
        if (i % 2 != 0) {
          Serial.print(" Msg n°");
          Serial.print((i/2)+1);
          Serial.print(" : ");
          Serial.print(msgpacket[i],HEX);
          Serial.print(" ");
        }
        else {
          Serial.print(msgpacket[i],HEX);
        }
      }
      Serial.print(" CRC : ");
      Serial.print(crc_local.part[0],HEX);
      Serial.print(" ");
      Serial.println(crc_local.part[1],HEX);


    }

  }


  nloop++;
}
