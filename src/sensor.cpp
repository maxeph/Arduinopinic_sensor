#include <Arduino.h>
#include <Manchester.h>  //Initialising 433 wireless library


#define DEBUG 1 // if 1, debug with Serial
#define TX_433 2 // Pin connecter to Transmitter


byte msgpacket[] = {1,2,3,4}; // init unsigned bytes to be sent over

union intarray { // shared memory for int and byte array to get its bytes
  int ints;
  byte part[2];
};

void float2int(float *farg, int *intresult) { // Convert float 2 a signed int for 2 decimal precision
  *intresult = *farg * 100;
}

int bytes2int(byte arg[2]) { // Convert a 2byte array into int
intarray result;
result.part[0] = arg[0];
result.part[1] = arg[1];
return result.ints;
}

void buildpacket(byte msg[4], byte part1[2], byte part2[2]) { // build array to be sent
  msg[0] = part1[0];
  msg[1] = part1[1];
  msg[2] = part2[0];
  msg[3] = part2[1];
}



/* to delete when setting up sensors */

float tempext = 3.42;
float tempeau = 17.24;
intarray itempext, itempeau;

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
    for(int i=0;i<4;i++) {
    Serial.println(msgpacket[i]);
    }
  }

}

void loop() {
  if (DEBUG) {
  man.transmit(0x01); // Test transmitter
  delay(1000);
}
}
