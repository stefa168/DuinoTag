#define IR_HEAD 2000
#define IR_SPACE 600
#define IR_ONE 1200
#define IR_ZERO 600

#define SHOT_LENGTH 16

// BIT infos:
byte datas[][SHOT_LENGTH] = { 
                   {1, 0,1,1,0,0,1,1, 1,0,1, 0,1,1,0},
                   {1, 0,0,0,0,0,0,0, 0,0,0, 0,0,0,0},
                   {1, 0,0,0,0,0,0,0, 0,0,0, 0,0,0,0},
                   {1, 0,0,0,0,0,0,0, 0,0,0, 0,0,0,0},
                   {1, 0,0,0,0,0,0,0, 0,0,0, 0,0,0,0}
                   };

int IRledPin = 11;

int count = 0;

void setup(){
  pinMode(IRledPin, OUTPUT);
  pinMode(13, OUTPUT);
  
  digitalWrite(13, LOW);
  
  Serial.begin(115200);
  Serial.println("Ready to send!");
  delay(5000);
}

void loop(){
  Serial.print("Code Number ");
  Serial.println(count);
  digitalWrite(13, HIGH); // Let's just turn on the LED on the board to show that we are sending something!
  SendCode();
  digitalWrite(13, LOW);  // Now we turn off the LED.
  delay(6000);
  count++;
}

void pulseIR(int microsecs) {
  cli();  // Let's stop any interrupt
  
  while(microsecs >= 0) {
    digitalWrite(IRledPin, HIGH);  // To do this action 3 microseconds will pass.
    delayMicroseconds(11); 
    digitalWrite(IRledPin, LOW);  // Again other 3 microseconds
    delayMicroseconds(11);
    
    // This number isn't exactly the sum of the 4 values up there; 
    // it's trimmed to make the timing of the receiver more accurate.
    // The best value that I noticed is about 29 microseconds.
    
    // EDIT: After using a Frequency Meter I found out that it is 
    // better to transmit at about 11 microseconds of delay and
    // to subtract exactly 28 from the counter. By doing this the 
    // frequency is  almost exactly at 57 kHZ.
    
    // EDIT 2: Again, now the best time to subtract is 30 microsecs because it results in less erorrs
    microsecs -= 30;
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
  
  for(uint8_t i=0; i<SHOT_LENGTH; i++) {
    switch(datas[0][i]){
      case 0: pulseIR(IR_ZERO);
              break;
      case 1: pulseIR(IR_ONE);
              break;
    }
  }
}
