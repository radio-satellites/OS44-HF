#include <EEPROM.h>
#include "SoftwareSerial.h"
#include "Morse.h"


const int Rx = 3; 

const int Tx = 4;

SoftwareSerial mySerial(Rx, Tx);

void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}
String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0';
  return String(data);
}


char message[100] = "BALLOON BEACON ";
char callsign[20] = "VE3SVF";

void setup() {
 pinMode(0,OUTPUT);
 pinMode(Rx, INPUT);
 pinMode(Tx, OUTPUT);

 mySerial.begin(9600);

 String retrievedString = readStringFromEEPROM(0);
  if (retrievedString.length() < 2){
    //Nothing in there. Let's write our own callsign 
    writeStringToEEPROM(0, "VE3SVF");
    //No need to write to our default char array, as the same string is already in there
  }
  else{
    //Nope, there's already a default callsign in here. Let's set our startup message to it
    retrievedString.toCharArray(callsign, sizeof(callsign));    
  }

  //Now, let's concat that together

  strcat(message,callsign);

}

void loop() {
  sendmsg(message);
  if (mySerial.available() != 0) {
    String callsign_temp = Serial.readString();
    callsign_temp.trim();    
    callsign_temp.toCharArray(callsign, sizeof(callsign));
    mySerial.println("OK");
  }
}
