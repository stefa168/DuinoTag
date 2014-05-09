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
  
  Serial.begin(9600);
  Serial.println("Pronto per inviare!");
}

void loop(){
  digitalWrite(13, HIGH);
  SendCode();
  digitalWrite(13, LOW);
  delay(10000);
}

void pulseIR(long microsecs) {
  cli();  // Blocchiamo tutti gli interrupt
  
  while(microsecs > 0) {
    digitalWrite(IRledPin, HIGH);  // Ci vogliono circa 3 microsecondi
    delayMicroseconds(7);
    digitalWrite(IRledPin, LOW);
    delayMicroseconds(7);
    
    // Quindi sono passati circa 26 microsecondi.
    microsecs -= 29;
  }
  
  pauseIR(IR_SPACE);
  
  sei();  // Riattiviamo gli interrupt
}

void pauseIR(long microsecs) {
  while(microsecs > 0) {
    delayMicroseconds(10);
    microsecs -= 10;
  }
}

void SendCode(){
  pulseIR(IR_HEAD);
  
  pulseIR(IR_ONE);
  pulseIR(IR_ZERO);
  pulseIR(IR_ONE);

  /*for(int i=0; i<sizeof(msg); i++){
    switch(msg[i]){
      case true:  pulseIR(IR_ONE);
                 break;
      case false: pulseIR(IR_ZERO);
                 break;
    }
  }*/
}
