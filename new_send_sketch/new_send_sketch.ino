#include <IRremote.h>

IRsend irsend;


byte tempPlayerID;
byte tempPlayerTeam;
byte tempDamageID;
//                0  1  2  3  4  5   6   7   8   9   10  11  12  13  14  15
byte damages[] = {1, 2, 4, 5, 7, 10, 15, 17, 20, 25, 30, 35, 40, 50, 75, 100};


unsigned long dataToSend;


void setup()
{
  pinMode(13, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  
  Serial.println("Shot command:");
  while(1){
    Serial.println("Insert Player ID (MUST be between 0 and 127)");
    waitForInput();
    tempPlayerID = Serial.parseInt();
    if(tempPlayerID <= 127 && tempPlayerID >= 0){
      Serial.print(tempPlayerID);
      Serial.println(": OK.");
      break;
    }
  }
  
  while(1){
    Serial.println("Insert Team ID (MUST be between 0 and 7)");
    waitForInput();
    tempPlayerTeam = Serial.parseInt();
    if(tempPlayerTeam <= 7 && tempPlayerID >= 0){
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
    waitForInput();
    tempDamageID = Serial.parseInt();
    if(tempDamageID <= 15 && tempPlayerID >= 0){
      // We just save the correct damage.
      Serial.print(tempDamageID);
      Serial.println(": OK.");
      break;
    }
  }
  
  // Reset the data to send
  dataToSend = 0;
  
  //Start with the PlayerID
  for(int i=0; i<7; i++){
    if(tempPlayerID & 0x40){
      dataToSend = (dataToSend << 1) | 1;
    } else {
     dataToSend <<= 1;
    }
    Serial.println(dataToSend, BIN);
    tempPlayerID <<= 1;
  }
  
  // Now it's the time for TeamID
  for(int i=0; i<3; i++){
    if(tempPlayerTeam & 0x4){
      dataToSend = (dataToSend << 1) | 1;
    } else {
     dataToSend <<= 1;
    }
    Serial.println(dataToSend, BIN);
    tempPlayerTeam <<= 1;
  }
  
  // Finally the damage.
  for(int i=0; i<4; i++){
    if(tempDamageID & 0x8){
      dataToSend = (dataToSend << 1) | 1;
    } else {
     dataToSend <<= 1;
    }
    Serial.println(dataToSend, BIN);
    tempDamageID <<= 1;
  }
  
  
  digitalWrite(13, HIGH);
  irsend.sendDtag(dataToSend, 16, 38);
  digitalWrite(13, LOW);
  Serial.println("Sent.\n");
  
}

void waitForInput(){
  while(Serial.available() == 0){
    // Just wait.
  }
}
