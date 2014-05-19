#include <IRremote.h>

IRsend irsend;

unsigned long data;

void setup()
{
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  
  digitalWrite(13, HIGH);
  irsend.sendDtag(0x9E1E, 16, 56);
  digitalWrite(13, LOW);
  delay(10000);
  
}
