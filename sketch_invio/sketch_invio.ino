#define IR_HEAD 2000
#define IR_SPACE 300
#define IR_ONE 1200
#define IR_ZERO 600

int IRledPin = 11;
int run = 0;

void setup(){
  pinMode(IRledPin, OUTPUT);
  pinMode(13, OUTPUT);
  
  digitalWrite(13, LOW);
  
  Serial.begin(115200);
  Serial.println("Ready to send!");
}

void loop(){
  digitalWrite(13, HIGH); // Let's just turn on the LED on the board to show that we are sending something!
  SendCode();
  digitalWrite(13, LOW);  // Now we turn off the LED.
  delay(10000);
}

void pulseIR(long microsecs) {
  cli();  // Let's stop any interrupt
  
  while(microsecs > 0) {
    digitalWrite(IRledPin, HIGH);  // To do this action 3 microseconds will pass.
    delayMicroseconds(7); 
    digitalWrite(IRledPin, LOW);  // Again other 3 microseconds
    delayMicroseconds(7);
    
    // This number isn't exactly the sum of the 4 values up there; 
    // it's trimmed to make the timing of the receiver more accurate.
    // The best value that I noticed is about 29 microseconds.
    microsecs -= 29;
  }
  
  pauseIR(IR_SPACE);
  
  sei();  // Now we reactivate any interrupt
}


// This function is needed to have a pause after any type of IR bit that has been sent so the receiver
// can read all.
void pauseIR(long microsecs) {
  while(microsecs > 0) {
    // 10 microsecs looked like a good delay value.
    delayMicroseconds(10);
    microsecs -= 10;
  }
}

// For now this function is predefined but I'll modify it 
// so that we can prepare a code in an array and then we can send it.
void SendCode(){
  // The IR head identifies the communication and should be as unique as possible, but it must
  // stay the same for all the time.
  pulseIR(IR_HEAD);
  
  // The code.
  pulseIR(IR_ONE);
  pulseIR(IR_ZERO);
  pulseIR(IR_ONE);
  
  // I tried to do something useful, but for now we won't use it.
  /*for(int i=0; i<sizeof(msg); i++){
    switch(msg[i]){
      case true:  pulseIR(IR_ONE);
                 break;
      case false: pulseIR(IR_ZERO);
                 break;
    }
  }*/
}
