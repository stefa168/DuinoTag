

#define DEBUG

#define LCD_DELAY 300
#define BATTERY_DELAY 60

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

// From full to empty
byte battery[][8] = {{ 0b01110,
                       0b11111,
                       0b11111,
                       0b11111,
                       0b11111,
                       0b11111,
                       0b11111,
                       0b11111 },
                       
                     { 0b01110,
                       0b11111,
                       0b10001,
                       0b11111,
                       0b11111,
                       0b11111,
                       0b11111,
                       0b11111 },
                       
                     { 0b01110,
                       0b11111,
                       0b10001,
                       0b10001,
                       0b11111,
                       0b11111,
                       0b11111,
                       0b11111 },
                     
                     { 0b01110,
                       0b11111,
                       0b10001,
                       0b10001,
                       0b10001,
                       0b11111,
                       0b11111,
                       0b11111 },
                       
                     { 0b01110,
                       0b11111,
                       0b10001,
                       0b10001,
                       0b10001,
                       0b10001,
                       0b11111,
                       0b11111 }
                    };


// Packet Section
unsigned long receivedData = 0;
unsigned int receivedDataLength = 0;

// Shot Section
byte receivedPlayerID = 0;
byte receivedPlayerTeam = 0;
byte receivedDamageID = 0;
  
// Command (3 bytes) Section
byte receivedCommandID = 0;
byte receivedCommandData = 0;
byte receivedCommandEnd = 0;

// End Packet Section


// Player Section
unsigned long toSendData = 0;
byte playerID = 0;
int playerHealth = 0;
byte playerTeam = 1; // Team 1 is NULL team / dead team
byte playerArmor = 0;
int ammo = 0;

const int startHealth = 100;
const int startAmmo = 50;
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

unsigned long batteryTimer = 0;
unsigned long LCDtimer = 0;

int voltage = 0;
byte charge = 0;
// End System Section


// System Flags Section
byte state = 0; // 0 Means Startup.
byte dataInfo = 0;
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
  lcd.createChar(1, battery[4]);
  lcd.createChar(2, battery[3]);
  lcd.createChar(3, battery[2]);
  lcd.createChar(4, battery[1]);
  
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
    delay(230);
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
  
  updateBattery();
  
  LCDtimer = millis() + LCD_DELAY;
}

void loop() {
  resetReceivedData();
  
  getNewRawData();
  
  analyzeRawData();
  
  switch(dataInfo){
    // Case 0: we got an error! this means that a code isn't acceptable or something else, so we skip the player updates part.
    case 0: break;
    
    // We got a shot command!
    case 1: Serial.println("Got a shot command!");
            break;
    
    // We got a command
    case 2: Serial.println("Got a command!");
            break;
  }
  
  if(state != 1){
    updateBattery();
  
    // We update the LCD with new informations, but we do this only at the end!
    updateLCD();
  }
  
}

void applyCommand(){
  /*
  byte receivedCommandID = 0;
  byte receivedCommandData = 0;
  byte receivedCommandEnd = 0;
  */
  switch(receivedCommandID){
   case 0x80:  addHealth(receivedCommandData);
               break;
           
   case 0x81:  addAmmo(receivedCommandData);
               break;
               
   case 0x82:  //setTeam();
               break;
               
   case 0x83:  singleCommand();
               break;
  }
}

void singleCommand(){
  switch(receivedCommandID){
    // Admin kill
    case 0x00:  addHealth(-playerHealth);
                break;
    // Pause / Unpause
    case 0x01:  //TODO
                break;
    // Start Game
    case 0x02:  //TODO
                break;
    // Restore defaults
    case 0x03:  //TODO
                break;
    // Respawn
    case 0x04:  //TODO
                break;
    // Immediate new game
    case 0x05:  //TODO
                break;
    // Full Ammo
    case 0x06:  addAmmo(startAmmo);
                break;
    // End Game
    case 0x07:  //TODO
                break;
    // Reset Clock
    case 0x08:  //TODO
                break;
    // Initialize Player
    case 0x0a:  state = 2;  // State 2 means that we are initializing but not ready for game
                addHealth(startHealth);
                addAmmo(startAmmo);
                break;
  }
}

void setTeam(byte team){
  // We update team only if state is 2.
  if(state == 2){
    playerTeam = receivedCommandData;
    applyColor(receivedCommandData);
  }
}

void addAmmo(int loc_ammo){
  #ifdef DEBUG
  Serial.println("Adding ammo");
  #endif
  ammo += loc_ammo;
}

void addHealth(int loc_health){
  #ifdef DEBUG
  Serial.println("Adding health");
  #endif
  playerHealth += loc_health;
}

void getNewRawData(){
  
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
}

void updateLCD(){
  // We mustn't update very often, otherwise the screen becomes unreadable!
  if(LCDtimer > millis()){
    return;
  }
  
  lcd.clear();
  
  // Battery section
  lcd.setCursor(15,0);
  lcd.write(byte(charge));
  
  if(voltage != 100){
    lcd.print(" ");
  }
  
  lcd.print(voltage);
  lcd.write("%");
  
  LCDtimer = millis() + LCD_DELAY;
}

void updateBattery(){
  if(millis() > batteryTimer){
    batteryTimer = millis() + BATTERY_DELAY;
    // First read to clean out the sensor
    analogRead(A0);
    voltage = analogRead(A0);
    voltage = constrain(voltage, 50, 900);
    voltage = map(voltage, 50, 900, 0, 100);
  
    charge = map(voltage, 0, 100, 1, 5);

  }
}

void analyzeRawData(){
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
    
    /*
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
    */
    
    // 1 means shot
    dataInfo = 1;
    
  } else if(receivedDataLength == 24){
    // We received a command!
    // Let's analize the first byte.
    for(int i=0; i<8; i++){
      if(receivedData & 0x800000){
        receivedCommandID = (receivedCommandID << 1) | 1;
      } else {
        receivedCommandID <<= 1;
      }
      receivedData <<= 1;
    }
    // Now the second.
    for(int i=0; i<8; i++){
      if(receivedData & 0x800000){
        receivedCommandData = (receivedCommandData << 1) | 1;
      } else {
        receivedCommandData <<= 1;
      }
      receivedData <<= 1;
    }
    // Now the end. If the end doesn't match 0xE8 we trash the code.
    for(int i=0; i<8; i++){
      if(receivedData & 0x800000){
        receivedCommandEnd = (receivedCommandEnd << 1) | 1;
      } else {
        receivedCommandEnd <<= 1;
      }
      receivedData <<= 1;
    }
    
    if(receivedCommandEnd != 0xE8){
      #ifdef DEBUG
      Serial.println("This code doesn't end correctly! Trashed.");
      #endif
      // 0 means error, so we return this.
      dataInfo = 0;
      return;
    }
    // 2 means command
    dataInfo = 2;
    
  } else {
    // No message or something strange happened, so we return 0
    dataInfo = 0;
  }
  
  return;
  
}

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
  if(receivedDataLength){
    // General section
    receivedData = 0;
    receivedDataLength = 0;
    
    // Shot Section
    receivedPlayerID = 0;
    receivedPlayerTeam = 0;
    receivedDamageID = 0;
    
    // Command section
    receivedCommandID = 0;
    receivedCommandData = 0;
    receivedCommandEnd = 0;
  }
}
