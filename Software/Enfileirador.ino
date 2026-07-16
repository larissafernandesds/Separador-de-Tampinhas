#include <Servo.h>

Servo servo;

void setup() {
  servo.attach(9);
}

void loop() {
  // Quanto mais perto de servo.write(69), mais devagar. Esse é o limiar entre horário e antihorario 
  
  servo.write(200); // antihorario 
  delay(500);
  servo.write(20); // horário 
  delay(100);
}
