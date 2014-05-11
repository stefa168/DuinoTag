
//uint8_t IRpin = 2;
// Digital pin #2 is the same as Pin D2 see
// http://arduino.cc/en/Hacking/PinMapping168 for the 'raw' pin mapping
#define IRpin_PIN   PIND
#define IRpin       2

// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 1500
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
int data[MAXPULSES]; // pair is high and low pulse
int numberpulses; // index for pulses we're storing

void setup(void) {
  Serial.begin(115200);
  Serial.println("Ready to decode IR!");
}

void loop(void) {
  // Reset the number of pulses!
  numberpulses = 0;
  
  
  // If the code isn't correct or is a diffrent code (like one of a remote) we just delete all and return null so we restart.
  if(listenForIR()){
    Serial.println("An error occurred while converting.");
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
}


// Function for IR listening. In a future version I might try to make all this multitasking so I can relax more.
int listenForIR(void) {
  // Let's keep counting until we don't return the number of pulses recorded.
  while (1) {
    int highpulse, lowpulse;  // Temporary storage timing
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
         return false;
       }
    }
 
    // Same as above.
    while (! (IRpin_PIN & _BV(IRpin))) {
       // pin is still LOW
       lowpulse++;
       delayMicroseconds(RESOLUTION);
       if ((lowpulse >= MAXPULSE)  && (numberpulses != 0)) {
         return false;
       }
    }
    
    // But this time we compare the received code to the diffrent 
    // ones so we get an array alerady complete
    // of all we need to work!
    
    int oncode = lowpulse * RESOLUTION;
    
    if(numberpulses == 0){
      if(abs(oncode - IR_HEAD) > (oncode * FUZZINESS / 100)){
        return true;
      } 
    } else if(abs(oncode - IR_ONE) <= (oncode * FUZZINESS / 100)){
      data[numberpulses-1] = 1;
    } else if(abs(oncode - IR_ZERO) <= (oncode * FUZZINESS / 100)){
      data[numberpulses-1] = 0;
    } else {
      return true;
    }
 
    // we read one high-low pulse successfully, continue!
    numberpulses++;
  }
}
