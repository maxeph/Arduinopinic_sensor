#include <Arduino.h>

#define DEBUG 1 // if 1, debug with Serial

uint8_t msgpacket[] = {1,2,3,4,5,6,7,8}; // init unsigned bytes to be sent over

void float2int(float *farg, int *intresult) { // Convert float 2 a signed int for 2 decimal precision
  *intresult = *farg * 100;
}

struct intarray { // shared memory for int to get its bytes
  int ints;
  uint8_t bytes[2];
};

/* to delete when setting up sensors */

float tempext = 3.42;
float tempeau = 17.24;
intarray itempext;
intarray itempeau;
/* */

void setup() {
  if (DEBUG) {  // Sending over Serial to make sure it works
    Serial.begin(9600);
    Serial.println("Initialising debug mode...");

    float2int(&tempext, &itempext.ints); // converting floats to int and storing in structs objects
    float2int(&tempeau, &itempeau.ints);

    Serial.print("Outside temperature (°C) : ");
    Serial.println(float(itempext.ints)/100);
    Serial.print('In bytes : ');
    Serial.print(itempext.bytes[0],HEX);
    Serial.print(" ");
    Serial.println(itempext.bytes[1],HEX);
    Serial.print("Water temperature (°C) : ");
    Serial.println(float(itempeau.ints)/100);
    Serial.print('In bytes : ');
    Serial.print(itempeau.bytes[0],HEX);
    Serial.print(" ");
    Serial.println(itempeau.bytes[1],HEX);
  }

}

void loop() {
  // put your main code here, to run repeatedly:
}
