/*
 * IRremote: IRrecvDump - dump details of IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 * JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
 */

//#define DEBUG

// Receiver section
#include <IRremote.h>
int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;
// End Receiver section


#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 8, 2, 4, 10, 12);

byte cuore[8] = {  0b00000,
                   0b00000,
                   0b01010,
                   0b11111,
                   0b11111,
                   0b01110,
                   0b00100,
                   0b00000
                };


// Packet Section
unsigned long receivedData = 0;
unsigned int receivedDataLength = 0;

// Shot Section
byte receivedPlayerID = 0;
byte receivedPlayerTeam = 0;
byte receivedDamageID = 0;
  
// Command Section


// End Packet Section


// Player Section
unsigned long toSendData = 0;
byte playerID = 0;
byte playerLife = 0;
byte playerTeam = 1; // Team 1 is NULL team / dead team
byte playerArmor = 0;
byte weapon = 0;
// End Player Section


// System Section
#define sysID 0  // Unique ID!
//                0  1  2  3  4  5   6   7   8   9   10  11  12  13  14  15
byte damages[] = {1, 2, 4, 5, 7, 10, 15, 17, 20, 25, 30, 35, 40, 50, 75, 100};
byte teamColor[][3] = { {255, 255, 255}, // White/Admin/Referee Team
                        { 0,   0,   0 }, // Dead/NULL Team
                        {255,  0,   0 }, // Red Team
                        { 0,  255,  0 }, // Green Team
                        { 0,   0,  255}, // Blue Team
                        {255, 128,  0 }, // Yellow Team
                        { 0,  255, 255}, // Cyan Team
                        {128,  0,  255}  // Violet Team
                      };
                      
String teamNames[] = {"Admin",
                      "Dead",
                      "Red",
                      "Green",
                      "Blue",
                      "Yellow",
                      "Cyan",
                      "Violet"
                      };
                   
// End System Section


// System Flags Section
byte state = 0; // 0 Means Startup.
byte error = 0;
// End System Flags Section


// Connection Settings
  //Connessioni RGB
#define RledPIN 5
#define GledPIN 6
#define BledPIN 9
// End COnnection Settings

void setup()
{
  lcd.begin(20, 4);
  
  lcd.createChar(0, cuore);
  
  lcd.setCursor(3,1);
  lcd.print("Starting up...");
  
  delay(900);
  
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("Version 1.3 R1");
  #endif
  
  lcd.setCursor(3,2);
  lcd.print("Version 1.3 R1");
  
  delay(2100);
  
  lcd.clear();
  lcd.setCursor(3,1);
  lcd.print("Testing leds..");
  
  pinMode(RledPIN, OUTPUT);
  pinMode(GledPIN, OUTPUT);
  pinMode(BledPIN, OUTPUT);
  
  for(int i=0; i<8; i++){
    applyColor(i);
    delay(300);
  }
  applyColor(1);
  lcd.clear();
  lcd.setCursor(3,1);
  lcd.print("Starting IR..");
  irrecv.enableIRIn(); // Start the receiver
  lcd.setCursor(7,2);
  lcd.print("Done");
  state = 1; // 1 Means Stand-by State.
  delay(800);
  lcd.clear();
}

void loop() {
  if(receivedDataLength)
    resetReceivedData();
  
  if (irrecv.decode(&results)) {
    receivedData = results.value;
    receivedDataLength = results.bits;
    #ifdef DEBUG
    Serial.println("\n\nNew Code:");
    Serial.print("Length: ");
    Serial.println(receivedDataLength);
    Serial.print("HEX: ");
    Serial.println(receivedData, HEX);
    Serial.print("DEC: ");
    Serial.println(receivedData, DEC);
    Serial.print("BIN: ");
    Serial.println(receivedData, BIN);
    #endif
    irrecv.resume(); // Receive the next value
  }
  
  // User
  if(receivedDataLength == 16){
    for(int i=0; i<7; i++){
      if(receivedData & 0x2000){
        receivedPlayerID = (receivedPlayerID << 1) | 1;
      } else {
        receivedPlayerID <<= 1;
      }
      receivedData <<= 1;
    }
    #ifdef DEBUG
    Serial.print("Player ID: ");
    Serial.println(receivedPlayerID);
    #endif
    
    // Team
    for(int i=0; i<3; i++){
      if(receivedData & 0x2000){
        receivedPlayerTeam = (receivedPlayerTeam << 1) | 1;
      } else {
        receivedPlayerTeam <<= 1;
      }
      receivedData <<= 1;
    }
    #ifdef DEBUG
    Serial.print("Team ID: ");
    Serial.print(receivedPlayerTeam);
    Serial.print("(");
    Serial.print(teamNames[receivedPlayerTeam]);
    Serial.println(")");
    #endif
    
    // Damage
    for(int i=0; i<4; i++){
      if(receivedData & 0x2000){
        receivedDamageID = (receivedDamageID << 1) | 1;
      } else {
        receivedDamageID <<= 1;
      }
      receivedData <<= 1;
    }
    #ifdef DEBUG
    Serial.print("Damage ID: ");
    Serial.print(receivedDamageID);
    Serial.print("(");
    Serial.print(damages[receivedDamageID]);
    Serial.println(")");
    #endif
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Hit!");
    lcd.setCursor(0,1);
    lcd.print("Player ");
    lcd.print(receivedPlayerID);
    lcd.setCursor(0,2);
    lcd.print(teamNames[receivedPlayerTeam]);
    lcd.setCursor(0,3);
    lcd.print("-");
    lcd.print(damages[receivedDamageID]);
    lcd.print(" ");
    lcd.write(byte(0));
    
  }
  
  
}


/*
void dump(decode_results *results) {
  int count = results->rawlen;
  if (results->decode_type == DTAG) {
    Serial.print("Decoded DTAG: ");
  } 
  
  Serial.print(results->value, HEX);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");
}
*/

void applyColor(byte team){
  if(team < 0 || team > 7){
    // TODO: Add error.
    return;
  }
  rgbColor(teamColor[team][0], teamColor[team][1], teamColor[team][2]);
}

void rgbColor(byte r, byte g, byte b){
  analogWrite(RledPIN, r);
  analogWrite(GledPIN, g);
  analogWrite(BledPIN, b);
}

void resetReceivedData(){
  // General section
  receivedData = 0;
  receivedDataLength = 0;
  
  // Shot Section
  receivedPlayerID = 0;
  receivedPlayerTeam = 0;
  receivedDamageID = 0;
}
