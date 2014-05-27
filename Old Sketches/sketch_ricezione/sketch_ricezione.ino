
//uint8_t IRpin = 2;
// Digital pin #2 is the same as Pin D2 see
// http://arduino.cc/en/Hacking/PinMapping168 for the 'raw' pin mapping
#define IRpin_PIN   PIND
#define IRpin       2

// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 2500
#define MAXPULSES 50

// what our timing resolution should be, larger is better
// as its more 'precise' - but too large and you wont get
// accurate timing
#define RESOLUTION 20

// What percent we will allow in variation to match the same code
#define FUZZINESS 23

#define IR_HEAD 2000
#define IR_SPACE 600
#define IR_ONE 1200
#define IR_ZERO 600

// we will store up to MAXPULSES
uint16_t data[MAXPULSES+2]; // MAXPULSES + 2 because 1 pulse is the time before 
                            // the start of the reading and the other one is the header
                            
uint8_t numberpulses; // index for pulses we're storing

uint16_t highpulse, lowpulse;

// Error 0 means no error.
int error = 0;

// State 0 means initialisation
int state = 0;

int count = 0;

// Data of player
// The UNique ID is used to identificate a tagger system. It must be unique!
#define UN_ID 0

// Data of shot packet.
byte rec_shotid;
byte rec_team;
byte rec_damage;

byte damages[] = {1,2,4,5,7,10,15,17,20,25,30,35,40,50,75,100};

void setup(void) {
  Serial.begin(115200);
  Serial.println("Version 1.3 R1");
  Serial.println("Ready to decode IR!");
  
  // Test for RGB output.
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  analogWrite(5, 255);
  delay(500);
  analogWrite(5, 0);
  analogWrite(6, 255);
  delay(500);
  analogWrite(6, 0);
  analogWrite(9, 255);
  delay(500);
  analogWrite(9, 0);
  // End of test.
  
  // State 1 = ended initialisation and starting to use StandByMode
  state = 1;
}

void loop(void) {
  
  listenForIR();
  
  // If the code isn't correct or is a diffrent code (like one of a remote) we just delete all and return null so we restart.
  switch(error){
    case 1: Serial.print("Header error: ");
            Serial.println(lowpulse*RESOLUTION);
            return;
    case 2: Serial.print("No match error: ");
            Serial.print(lowpulse*RESOLUTION);
            Serial.print(" Pulses: ");
            Serial.println(numberpulses);
            return;
  }
  
  switch(state){
    case 0: state = 1; 
            break;
    case 1: ModeStandBy();
            break;
  }
  
  /*
  // Reset the number of pulses!  
  numberpulses = 0;
  Serial.println();
  Serial.print("Code Number ");
  Serial.println(count);
  count++;
  
  numberpulses--;
  Serial.print("New Signal: ");
  Serial.print(numberpulses);
  Serial.println(" pulses long.");
  
  for(int i=0; i<numberpulses; i++){
    Serial.print(data[i]);
  }
  Serial.println();
  
  if(data[0] == 0){
    Serial.print("SHOT ID ");
    shotid = data[1]*64 + data[2]*32 + data[3]*16 + data[4]*8 + data[5]*4 + data[6]*2 + data[7];
    Serial.print(shotid);
    Serial.print(" TEAM ");
    team = data[8]*4 + data[9]*2 + data[10];
    Serial.print(team);
    Serial.print(" DAM ");
    damage = data[11]*8 + data[12]*4 + data[13]*2 + data[14];
    Serial.print(damages[damage]);
    switch(data[15]){
      case 0: Serial.println(" T 1 .");
              break;
      case 1: Serial.println(" T 2 .");
              break;
    }
  } else {
    
  }*/
}

// StandByMode
void ModeStandBy(){
  Serial.println("STBYM");
}

// Function for IR listening. In a future version I might try to make all this multitasking so I can relax more.
void listenForIR(void) {
  // Reset the number of pulses.
  numberpulses = 0;
  // Reset the error.
  error = 0;
  // Let's keep counting until we don't return the number of pulses recorded.
  while (1) {
    //int highpulse, lowpulse;  // Temporary storage timing
    highpulse = lowpulse = 0; // Start out with no pulse length
 
//  while (digitalRead(IRpin)) { // this is too slow!
    while (IRpin_PIN & (1 << IRpin)) {
       // Pin is still HIGH
 
       // Count off another few microseconds
       highpulse++;
       delayMicroseconds(RESOLUTION);
 
       // If the pulse is too long, we 'timed out' - either nothing
       // was received or the code is finished, so print what
       // we've grabbed so far, and then reset
       if ((highpulse >= MAXPULSE) && (numberpulses != 0)) {
         return;
       }
    }
 
    // Same as above.
    while (! (IRpin_PIN & _BV(IRpin))) {
       // pin is still LOW
       lowpulse++;
       delayMicroseconds(RESOLUTION);
       if ((lowpulse >= MAXPULSE)  && (numberpulses != 0)) {
         return;
       }
    }
    
    // But this time we compare the received code to the diffrent 
    // ones so we get an array alerady complete
    // of all we need to work!
    
    uint16_t oncode = lowpulse * RESOLUTION;
    
    if(numberpulses == 0){
      if(abs(oncode - IR_HEAD) > (oncode * FUZZINESS / 100)){
        error = 1;
        return;
      } 
    } else if(abs(oncode - IR_ONE) <= (oncode * FUZZINESS / 100)){
      data[numberpulses-1] = 1;
    } else if(abs(oncode - IR_ZERO) <= (oncode * FUZZINESS / 100)){
      data[numberpulses-1] = 0;
    } else {
      error = 2;
      return;
    }
 
    // we read one high-low pulse successfully, continue!
    numberpulses++;
  }
}
