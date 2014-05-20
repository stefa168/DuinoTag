//#define DEBUG

#include <IRremote.h>

IRsend irsend;


byte tempPlayerID;
byte tempPlayerTeam;
byte tempDamageID;
//                0  1  2  3  4  5   6   7   8   9   10  11  12  13  14  15
byte damages[] = {1, 2, 4, 5, 7, 10, 15, 17, 20, 25, 30, 35, 40, 50, 75, 100};


unsigned long dataToSend;
unsigned int dataToSendLength;


void setup()
{
  pinMode(13, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  
  // Reset the data to send
  resetDataToSend();
  Serial.println("Shot or Command");
  int typeOfCommand = waitForInput();
  if(typeOfCommand == 'S' || typeOfCommand && 's'){
    Serial.println("Shot command:");
    while(1){
      Serial.println("Insert Player ID (MUST be between 0 and 127)");
      tempPlayerID = waitForIntInput();
      if(tempPlayerID <= 127 && tempPlayerID >= 0){
        addDataToSend(7, tempPlayerID, 0x40, true); // 0x40 == 0b 0100 0000 (7 bits)
        Serial.print(tempPlayerID);
        Serial.println(": OK.");
        break;
      }
    }
    
    while(1){
      Serial.println("Insert Team ID (MUST be between 0 and 7)");
      tempPlayerTeam = waitForIntInput();
      if(tempPlayerTeam <= 3 && tempPlayerTeam >= 0){ 
        addDataToSend(3, tempPlayerTeam, 0x4, true); // 0x4 == 0b 100
        Serial.print(tempPlayerTeam);
        Serial.println(": OK.");
        break;
      }
    }
    
    Serial.println("Damages");
    
    for(int i=0; i<sizeof(damages); i++){
      Serial.print(i);
      Serial.print(": ");
      Serial.println(damages[i]);
    }
    
    while(1){
      Serial.println("Insert Damage ID (MUST be between 0 and 15)");
      tempDamageID = waitForIntInput();
      if(tempDamageID <= 15 && tempDamageID >= 0){
        addDataToSend(4, tempDamageID, 0x8, true); // 0x8 == 0b 1000
        Serial.print(tempDamageID);
        Serial.println(": OK.");
        break;
      }
    }  
    
    digitalWrite(13, HIGH);
    irsend.sendDtag(dataToSend, 16, 38);
    digitalWrite(13, LOW);
    Serial.println("Sent.\n");
    
  } else if (typeOfCommand == 'S' || typeOfCommand && 's'){
    Serial.println("\nCommand.");
    for(int i=0x80; i<=0x8C; i++){
      Serial.println(i, HEX);
    }
  }
  
}

void resetDataToSend(){
  dataToSend = 0;
  dataToSendLength = 0;
  #ifdef DEGBUG
  Serial.println("Reset dataToSend");
  #endif
}

void addDataToSend(int bits, int value, int topBit, boolean autoIncrement){
  for(int i=0; i<bits; i++){
    if(value & topBit){
      dataToSend = (dataToSend << 1) | 1;
    } else {
     dataToSend <<= 1;
    }
    #ifdef DEBUG
    Serial.println(dataToSend, BIN);
    #endif
    value <<= 1;
  }
  if(autoIncrement){
    dataToSendLength += bits;
    #ifdef DEBUG
    Serial.print("\nAdded ");
    Serial.print(bits);
    Serial.println(" bits to the stack.");
    #endif
  }
}

int waitForInput(){
  while(Serial.available() == 0){
    // Just wait.
  }
  return Serial.read();
}

int waitForIntInput(){
  while(Serial.available() == 0){
    // Just wait.
  }
  return Serial.parseInt();
}
