
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
#define FUZZINESS 22

#define IR_HEAD 2000
#define IR_SPACE 600
#define IR_ONE 1200
#define IR_ZERO 600

// we will store up to MAXPULSES
uint16_t data[MAXPULSES]; // pair is high and low pulse
uint8_t numberpulses; // index for pulses we're storing

uint16_t highpulse, lowpulse;

int count = 0;

byte shotid;
byte team;
byte damages[] = {1,2,4,5,7,10,15,17,20,25,30,35,40,50,75,100};
byte damage;

void setup(void) {
  Serial.begin(115200);
  Serial.println("Version 1.3 R1");
  Serial.println("Ready to decode IR!");
}

void loop(void) {

  // Reset the number of pulses!  
  numberpulses = 0;
  Serial.println();
  Serial.print("Code Number ");
  Serial.println(count);
  count++;
  // If the code isn't correct or is a diffrent code (like one of a remote) we just delete all and return null so we restart.
  switch(listenForIR()){
    case 1: Serial.print("Header error: ");
            Serial.println(lowpulse*RESOLUTION);
            return;
    case 2: Serial.print("No match error: ");
            Serial.print(lowpulse*RESOLUTION);
            Serial.print(" Pulses: ");
            Serial.println(numberpulses);
            return;
  }
  numberpulses--;
  Serial.print("New Signal: ");
  Serial.print(numberpulses);
  Serial.println(" pulses long.");
  
  for(int i=0; i<numberpulses; i++){
    Serial.print(data[i]);
  }
  Serial.println();
  
  if(data[0] == 1){
    Serial.print("SHOT ID ");
    shotid = data[1]*128 + data[2]*64 + data[3]*32 + data[4]*16 + data[5]*8 + data[6]*4 + data[7]*2 + data[8];
    Serial.print(shotid);
    Serial.print(" TEAM ");
    team = data[9]*4 + data[10]*2 + data[11];
    Serial.print(team);
    Serial.print(" DAM ");
    damage = data[12]*8 + data[13]*4 + data[14]*2 + data[15];
    Serial.print(damages[damage]);
    switch(data[16]){
      case 0: Serial.println(" T 1 .");
              break;
      case 1: Serial.println(" T 2 .");
              break;
    }
  }
}


// Function for IR listening. In a future version I might try to make all this multitasking so I can relax more.
int listenForIR(void) {
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
         return 0;
       }
    }
 
    // Same as above.
    while (! (IRpin_PIN & _BV(IRpin))) {
       // pin is still LOW
       lowpulse++;
       delayMicroseconds(RESOLUTION);
       if ((lowpulse >= MAXPULSE)  && (numberpulses != 0)) {
         return 0;
       }
    }
    
    // But this time we compare the received code to the diffrent 
    // ones so we get an array alerady complete
    // of all we need to work!
    
    uint16_t oncode = lowpulse * RESOLUTION;
    
    if(numberpulses == 0){
      if(abs(oncode - IR_HEAD) > (oncode * FUZZINESS / 100)){
        return 1;
      } 
    } else if(abs(oncode - IR_ONE) <= (oncode * FUZZINESS / 100)){
      data[numberpulses-1] = 1;
    } else if(abs(oncode - IR_ZERO) <= (oncode * FUZZINESS / 100)){
      data[numberpulses-1] = 0;
    } else {
      return 2;
    }
 
    // we read one high-low pulse successfully, continue!
    numberpulses++;
  }
}
