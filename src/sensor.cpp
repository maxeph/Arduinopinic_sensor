#include <Arduino.h>
#include <Manchester.h>  //Initialising 433 wireless library
#include <FastCRC.h>  //CRCcheck library

// Declaring definitions

#define DEBUG 1 // if 1, debug with Serial
#define TX_433 2 // Pin connecter to Transmitter
#define MSGLEN 4 // Msg len is 4 = 2 signed int (2 bytes each)
#define PCKTLEN MSGLEN+3 // +1 for the lenght of the msgpacket +2 for CRC 16

// Declaring structs

union intarray { // shared memory for int and byte array to get its bytes
  int ints;
  byte part[2];
};

// Declaring variables

byte msgpacket[PCKTLEN] = {PCKTLEN}; // init unsigned bytes to be sent over
FastCRC16 CRC16; // init CRC16 object
intarray itempext, itempeau, crcc;

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

void buildpacket(byte msg[PCKTLEN], byte part1[2], byte part2[2]) { // build array to be sent
  msg[0] = PCKTLEN;
  msg[1] = part1[0];
  msg[2] = part1[1];
  msg[3] = part2[0];
  msg[4] = part2[1];
  msg[5] = 0;
  msg[6] = 0;
}




/* to delete when setting up sensors */

float tempext = 3.42;
float tempeau = 17.24;

/* */

void setup() {

  man.setupTransmit(TX_433, MAN_600); // Initialising 433 wireless

  if (DEBUG) {  // Sending over Serial to make sure it works
    Serial.begin(9600);
    Serial.println("Initialising debug mode...");

    float2int(&tempext, &itempext.ints); // converting floats to int and storing in union objects
    float2int(&tempeau, &itempeau.ints);

    Serial.print("Outside temperature (°C) : ");
    Serial.println(float(itempext.ints)/100);
    Serial.print("In bytes : ");
    Serial.print(itempext.part[0]);
    Serial.print(" ");
    Serial.println(itempext.part[1]);
    Serial.print("Water temperature (°C) : ");
    Serial.println(float(itempeau.ints)/100);
    Serial.print("In bytes : ");
    Serial.print(itempeau.part[0],HEX);
    Serial.print(" ");
    Serial.println(itempeau.part[1],HEX);

    buildpacket(msgpacket,itempext.part,itempeau.part);

    for(uint8_t i=0;i<sizeof(msgpacket);i++) {
      Serial.println(msgpacket[i],HEX);
    }

    crcc.ints = CRC16.ccitt(msgpacket, sizeof(msgpacket));
    msgpacket[5] = crcc.part[0];
    msgpacket[6] = crcc.part[1];

  }

  Serial.println(crcc.part[0],HEX);
  Serial.println(crcc.part[1],HEX);
}

void loop() {
  man.transmitArray(PCKTLEN, msgpacket);
  delay(5000);

}
