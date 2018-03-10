#include <Arduino.h>
#include <Manchester.h>  //Initialising 433 wireless library
#include <Crc16.h>  // CRCcheck library
#include <DHT.h> // DHT library

// Declaring definitions

#define DEBUG 1 // if 1, debug with Serial
#define TX_433 2 // Pin connecter to Transmitter
#define DHTPIN 3 // Pin DHT
#define DHTTYPE DHT22 // Pin DHT
#define NBPARAM 3 // Number of int sent
#define MSGLEN NBPARAM*2 // Msg len is 4 = 2 signed int (2 bytes each)
#define PCKTLEN MSGLEN+3 // +1 for the lenght of the msgpacket +2 for CRC 16

// Declaring structs

union intarray { // shared memory for int and byte array to get its bytes
  int ints;
  byte part[2];
};

// Declaring variables

byte msgpacket[PCKTLEN] = {PCKTLEN}; // init unsigned bytes to be sent over
intarray itempext, itempeau, ihumid, crc_local;
float tempext, tempeau = 17.24, humid; // floats to be used with sensors
DHT dht(DHTPIN, DHTTYPE); // Declaring sensor
int nloop = 1; // Counting n° Sending

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

  if (DEBUG) {  // Sending over Serial to make sure it works
    Serial.begin(9600);
    Serial.println("Initialising debug mode...");
  }

  // Here to move in loop when sensors in place

  float2int(&tempeau, &itempeau.ints); // converting floats to int and storing in union objects

}

void loop() {
  delay(5000);
  tempext = dht.readTemperature();
  humid = dht.readHumidity();
  if (isnan(tempext) || isnan(humid)) {
    if (DEBUG) {
      Serial.println("Failed to read from DHT sensor!");
      dht.begin();
    }
    return;
  }

  float2int(&tempext, &itempext.ints); // converting floats to int and storing in union objects
  float2int(&humid, &ihumid.ints);

  if (DEBUG) { // Showing value received from sensors
    Serial.print("########## PACKET N° "); // Showing raw data
    Serial.print(nloop);
    Serial.println(" #################");
    Serial.print("Outside temperature (°C) : ");
    Serial.println(float(itempext.ints)/100);
    Serial.print("In bytes : ");
    Serial.print(itempext.part[0],HEX);
    Serial.print(" ");
    Serial.println(itempext.part[1],HEX);
    Serial.print("Water temperature (°C) : ");
    Serial.println(float(itempeau.ints)/100);
    Serial.print("In bytes : ");
    Serial.print(itempeau.part[0],HEX);
    Serial.print(" ");
    Serial.println(itempeau.part[1],HEX);
    Serial.print("Humidity (%) : ");
    Serial.println(float(ihumid.ints)/100);
    Serial.print("In bytes : ");
    Serial.print(ihumid.part[0],HEX);
    Serial.print(" ");
    Serial.println(ihumid.part[1],HEX);
  }

  buildpacket(msgpacket,itempext.part,itempeau.part,ihumid.part); // Building packet to be sent over

  if (DEBUG) { // Showing raw data sent over
    Serial.println("----------------------------------------");
    Serial.println("Raw data : ");
    Serial.print("Packet length : ");
    Serial.println(msgpacket[0],HEX);
    for(uint8_t i=1;i<PCKTLEN-2;i++) {
      if (i % 2 != 0) {
        Serial.print("Msg n°");
        Serial.print((i/2)+1);
        Serial.print(" : ");
        Serial.print(msgpacket[i],HEX);
        Serial.print(" ");
      }
      else {
        Serial.println(msgpacket[i],HEX);
      }
    }
  }

  crc_local.ints = getcrc(msgpacket); // Calculating CRC16

  msgpacket[PCKTLEN-2] = crc_local.part[0]; // Including raw CRC in msgpacket
  msgpacket[PCKTLEN-1] = crc_local.part[1];


  if (DEBUG) { // Showing CRC
    Serial.print("CRC : ");
    Serial.print(crc_local.part[0],HEX);
    Serial.print(" ");
    Serial.println(crc_local.part[1],HEX);
  }

  man.transmitArray(PCKTLEN, msgpacket);
  delay(5000);
  nloop++;
}
