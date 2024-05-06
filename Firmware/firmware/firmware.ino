#include <EEPROM.h>
#include "Morse.h"
#include "CommandParser.h"

#define USE_PIN 0

typedef CommandParser<> MyCommandParser;

MyCommandParser parser;

int tx_period = 60;

char message_char[100] = "HF BALLOON BEACON";

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

void writeIntIntoEEPROM(int address, int number)
{ 
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

int readIntFromEEPROM(int address)
{
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}

void set_tx_msg(MyCommandParser::Argument *args, char *response) {
  Serial.print("CHGMSG:"); 
  Serial.println(args[0].asString);
  String new_message = args[0].asString;
  new_message.toCharArray(message_char, sizeof(message_char));
  strlcpy(response, "OK", MyCommandParser::MAX_RESPONSE_SIZE);
}


void set_per(MyCommandParser::Argument *args, char *response) {
  Serial.print("CHGTICK:"); 
  Serial.println(args[0].asDouble);
  int new_rate = args[0].asDouble;
  tx_period = new_rate;
  strlcpy(response, "OK", MyCommandParser::MAX_RESPONSE_SIZE);
}

void set_per_def(MyCommandParser::Argument *args, char *response) {
  Serial.print("CHGDEFTICK:"); 
  Serial.println(args[0].asDouble);
  int new_rate = args[0].asDouble;
  tx_period = new_rate;
  writeIntIntoEEPROM(EEPROM.length()-2, tx_period);
  strlcpy(response, "OK", MyCommandParser::MAX_RESPONSE_SIZE);
}

void set_def_tx_msg(MyCommandParser::Argument *args, char *response) {
  Serial.print("CHGDEFMSG:"); 
  Serial.println(args[0].asString);
  String new_message = args[0].asString;
  new_message.toCharArray(message_char, sizeof(message_char));
  writeStringToEEPROM(0, new_message);
  strlcpy(response, "OK", MyCommandParser::MAX_RESPONSE_SIZE);
}


void send_time() {
  for (int i = 0; i < tx_period; i++) {
    digitalWrite(USE_PIN, HIGH);
    delay(100);
    digitalWrite(USE_PIN, LOW);
    delay(900);
  }

}

void setup() {
  pinMode(USE_PIN, OUTPUT);
  Serial.begin(9600);
  parser.registerCommand("SETTX", "s", &set_tx_msg);
  parser.registerCommand("SETDEFTX", "s", &set_def_tx_msg);
  parser.registerCommand("SETTICK", "d", &set_per);
  parser.registerCommand("SETDEFTICK", "d", &set_per_def);

  //Do we have a default message already in memory? If not, let's shove in our default message

  String retrievedString = readStringFromEEPROM(0);
  if (retrievedString.length() < 2){
    //Serial.print("DEBUG: NO STRING. WRITING NEW ONE.");
    //Nothing in there. Let's write our own string in!
    writeStringToEEPROM(0, "HF BALLOON BEACON");
    //No need to write to our default char array, as the same string is already in there
  }
  else{
    //Nope, there's already a default message in here. Let's set our startup message to it
    //Serial.print("DEBUG: USING STRING ALREADY IN MEMORY.");
    //Serial.print(retrievedString);
    retrievedString.toCharArray(message_char, sizeof(message_char));    
  }

  //Now, let's do the same with our TX period

  int tx_period_temp = readIntFromEEPROM(EEPROM.length()-2);

  if (tx_period_temp == 0){
    writeIntIntoEEPROM(EEPROM.length()-2, 60);
  }
  else if (tx_period_temp == -1){
    //Then we need no clicks
    tx_period = 0;
  }
  else{
    tx_period = tx_period_temp;
  }

  //Serial.println("Number:");
  //Serial.print(tx_period_temp);
  
}

void loop() {
  sendmsg(message_char);
  send_time();

  //Check for incoming commands

  if (Serial.available()) {
    char line[128];
    size_t lineLength = Serial.readBytesUntil('\n', line, 127);
    line[lineLength] = '\0';

    char response[MyCommandParser::MAX_RESPONSE_SIZE];
    parser.processCommand(line, response);
    Serial.println(response);
  }
  
}
