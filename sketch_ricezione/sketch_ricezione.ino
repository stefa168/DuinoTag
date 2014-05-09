
//uint8_t IRpin = 2;
// Digital pin #2 is the same as Pin D2 see
// http://arduino.cc/en/Hacking/PinMapping168 for the 'raw' pin mapping
#define IRpin_PIN   PIND
#define IRpin       2

// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 15000
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

void setup(void) {
  Serial.begin(9600);
  Serial.println("Ready to decode IR!");
}

void loop(void) {
  int numberpulses;
  
  numberpulses = listenForIR();
  
  Serial.print("Ricevuto un segnale lungo ");
  Serial.print(numberpulses);
  Serial.println(" impulsi.");
  Serial.println("R\tC");
  for(int i=0; i<numberpulses-1; i++){
    int oncode  = pulses[i][1] * RESOLUTION;
    
    Serial.print(oncode);
    Serial.print(" - ");    
    
    // Evaluate teh type of code received.
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
    }
    
  }
  Serial.println();
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

// Deprecated. TO DELETE!
void printpulses(void) {
  Serial.println("\n\r\n\rReceived: \n\rOFF \tON");
  for (uint8_t i = 0; i < currentpulse; i++) {
    Serial.print(pulses[i][0] * RESOLUTION, DEC);
    Serial.print(" usec, ");
    Serial.print(pulses[i][1] * RESOLUTION, DEC);
    Serial.println(" usec");
  }
  
  // print it in a 'array' format
  Serial.println("int IRsignal[] = {");
  Serial.println("// ON, OFF (in 10's of microseconds)");
  for (uint8_t i = 0; i < currentpulse-1; i++) {
    Serial.print("\t"); // tab
    Serial.print(pulses[i][1] * RESOLUTION / 10, DEC);
    Serial.print(", ");
    Serial.print(pulses[i+1][0] * RESOLUTION / 10, DEC);
    Serial.println(",");
  }
  Serial.print("\t"); // tab
  Serial.print(pulses[currentpulse-1][1] * RESOLUTION / 10, DEC);
  Serial.print(", 0};");
}
