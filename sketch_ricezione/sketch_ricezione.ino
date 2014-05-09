
//uint8_t IRpin = 2;
// Digital pin #2 is the same as Pin D2 see
// http://arduino.cc/en/Hacking/PinMapping168 for the 'raw' pin mapping
#define IRpin_PIN   PIND
#define IRpin       2

// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 5000
#define MAXPULSES 50

// what our timing resolution should be, larger is better
// as its more 'precise' - but too large and you wont get
// accurate timing
#define RESOLUTION 20

// What percent we will allow in variation to match the same code
#define FUZZINESS 20

#define IR_HEAD 2000
#define IR_SPACE 300
#define IR_ONE 1200
#define IR_ZERO 600

// we will store up to MAXPULSES
uint16_t pulses[MAXPULSES][2]; // pair is high and low pulse
uint8_t currentpulse = 0; // index for pulses we're storing

int numberpulses;

void setup(void) {
  Serial.begin(9600);
  Serial.println("Ready to decode IR!");
}

void loop(void) {
  // Reset the number of pulses!
  numberpulses = 0;
  
  numberpulses = listenForIR();
  
  Serial.print("New Signal: ");
  Serial.print(numberpulses);
  Serial.println(" pulses long.");
  
  // If the code isn't correct or is a diffrent code (like one of a remote) we just delete all and return null so we restart.
  if(!convertIR()){
    Serial.println("An error occurred while converting.");
    return;
  }
  
  // Otherwise
  
  Serial.println();
}

boolean convertIR(void){
  // Receive - Compare
  Serial.println("R\tC");
  for(int i=0; i<numberpulses; i++){
    int oncode  = pulses[i][1] * RESOLUTION;
    
    Serial.print(oncode);
    Serial.print(" - ");    
    
    // Evaluate the type of code received.
    if(abs(oncode - IR_HEAD) <= (oncode * FUZZINESS / 100)){
      Serial.print(IR_HEAD);
      Serial.println(" HEAD");
    } else if(abs(oncode - IR_ONE) <= (oncode * FUZZINESS / 100)){
      Serial.print(IR_ONE);
      Serial.println(" ONE");
    } else if(abs(oncode - IR_ZERO) <= (oncode * FUZZINESS / 100)){
      Serial.print(IR_ZERO);
      Serial.println(" ZERO");
    } else {
      Serial.print("?");
      Serial.println(" ERR");
      // There is something that isn't correct!
      return false;
    }
  }
}

// Function for IR listening. In a future version I might try to make all this multitasking so I can relax more.
int listenForIR(void) {
  currentpulse = 0;
 
  // Let's keep counting until we don't return the number of pulses recorded.
  while (1) {
    uint16_t highpulse, lowpulse;  // temporary storage timing
    highpulse = lowpulse = 0; // start out with no pulse length
 
//  while (digitalRead(IRpin)) { // this is too slow!
    while (IRpin_PIN & (1 << IRpin)) {
       // pin is still HIGH
 
       // count off another few microseconds
       highpulse++;
       delayMicroseconds(RESOLUTION);
 
       // If the pulse is too long, we 'timed out' - either nothing
       // was received or the code is finished, so print what
       // we've grabbed so far, and then reset
       if ((highpulse >= MAXPULSE) && (currentpulse != 0)) {
         return currentpulse;
       }
    }
    // we didn't time out so lets stash the reading
    pulses[currentpulse][0] = highpulse;
 
    // same as above
    while (! (IRpin_PIN & _BV(IRpin))) {
       // pin is still LOW
       lowpulse++;
       delayMicroseconds(RESOLUTION);
       if ((lowpulse >= MAXPULSE)  && (currentpulse != 0)) {
         return currentpulse;
       }
    }
    pulses[currentpulse][1] = lowpulse;
 
    // we read one high-low pulse successfully, continue!
    currentpulse++;
  }
}
