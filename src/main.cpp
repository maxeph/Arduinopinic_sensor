#include <Arduino.h>

#define DEBUG 1 // if 1, debug with Serial

uint8_t msgpacket[] = {1,2,3,4,5,6,7,8}; // init unsigned bytes to be sent over

void setup() {
if (DEBUG) {  // Just to be sure
  Serial.begin(9600);
  Serial.println("Initialising debug mode...");
  }

}

void loop() {
    // put your main code here, to run repeatedly:
}

void float2int(float *farg, int *ret_int) {

  *ret_int = *farg * 100;

}
