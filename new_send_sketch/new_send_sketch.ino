//#define DEBUG

#define HZ_FREQ 38

#include <IRremote.h>

#define DEBUG

IRsend irsend;


byte tempPlayerID;
byte tempPlayerTeam;
byte tempDamageID;
//                0  1  2  3  4  5   6   7   8   9   10  11  12  13  14  15
byte damages[] = {
  1, 2, 4, 5, 7, 10, 15, 17, 20, 25, 30, 35, 40, 50, 75, 100};


unsigned long dataToSend;
unsigned long dataToSendLength;
int data1;


void setup()
{
  pinMode(13, OUTPUT);
  Serial.begin(115200);
}

void loop() {

  // Reset the data to send
  resetDataToSend();

  Serial.println("Shot or Command");
  char typeOfCommand = waitForInput();
  if(typeOfCommand == 'S' || typeOfCommand == 's'){
    Serial.println("Shot");
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
      if(tempPlayerTeam <= 7 && tempPlayerTeam >= 0){ 
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

    dataToSendLength += 2;

#ifdef DEBUG
    Serial.print("PCKT LNGHT ");
    Serial.println(dataToSendLength);
#endif

    digitalWrite(13, HIGH);
    sendData();
    digitalWrite(13, LOW);
    Serial.println("Sent.\n");

  } 
  else if (typeOfCommand == 'c' || typeOfCommand == 'C'){
    Serial.println("\nCommand.");
    for(int i=0x80; i<=0x8C; i++){
      Serial.println(i, HEX);
    }
    switch(waitForIntInput()){
    case 80:  
      Serial.println("Add Health. How much? (number from 0 to 255)");
      addDataToSend(8, 0x80, 0x80, true); //Let's add 0x80 to the code to send
      data1 = waitForIntInput();
      Serial.print(data1);
      data1 = constrain(data1, 0, 255);
      Serial.print(" -> ");
      Serial.println(data1);
      addDataToSend(8, data1, 0x80, true);
      addDataToSend(8, 0xE8, 0x80, true);
      break;

    case 81:  
      Serial.println("Add Rounds. How many? (number from 0 to 255)");
      addDataToSend(8, 0x81, 0x80, true);
      data1 = waitForIntInput();
      Serial.print(data1);
      data1 = constrain(data1, 0, 255);
      Serial.print(" -> ");
      Serial.println(data1);
      addDataToSend(8, data1, 0x80, true);
      addDataToSend(8, 0xE8, 0x80, true);
      break;

    case 82:
      Serial.print("Team Number");
      while(1){
        data1 = waitForIntInput();
        if(data1 >=0 && data1 <= 7){
          break;
        }
      }
      Serial.print(data1);
      addDataToSend(8, 0x82, 0x80, true);
      addDataToSend(8, data1, 0x80, true);
      addDataToSend(8, 0xE8, 0x80, true);
      break;

    case 83:  
      Serial.print("Command Number ");
      while(1){
        data1 = waitForIntInput();
        if(data1 >=0 && data1 <= 17){
          break;
        }
      }
      Serial.print(data1);
      addDataToSend(8, 0x83, 0x80, true);
      addDataToSend(8, data1, 0x80, true);
      addDataToSend(8, 0xE8, 0x80, true);
      break;
    }

    sendData();

  }

}

void sendData(){
  irsend.sendDtag(dataToSend, dataToSendLength, HZ_FREQ);
}

void resetDataToSend(){
  dataToSend = 0;
  dataToSendLength = 0;
  data1 = 20;
#ifdef DEBUG
  Serial.println("Reset dataToSend");
#endif
}

void addDataToSend(int bits, int value, int topBit, boolean autoIncrement){
  for(int i=0; i<bits; i++){
    if(value & topBit){
      dataToSend = (dataToSend << 1) | 1;
    } 
    else {
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

