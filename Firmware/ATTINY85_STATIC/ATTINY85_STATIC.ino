#include "Morse.h"

#define tx_period 30

void send_time() {
  for (int i = 0; i < tx_period; i++) {
    digitalWrite(0, HIGH);
    delay(100);
    digitalWrite(0, LOW);
    delay(900);
  }

}

void setup() {
  pinMode(0,OUTPUT);

}

void loop() {
  sendmsg("HF BALLOON BEACON");
  send_time();
}
