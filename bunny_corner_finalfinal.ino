//   Bunny Corner  21/10/20  Working 
//   M Beer
//   11/11/20 attention to not missing train leaving
//   TO DO   modify decide() with time override

//   LDR detectors - EOR section

//  pins A1 and A2 LDRs
//  pin 9  bunnies, pin 10 waving arm  motor shield stepper port #2 kite
//  ALARM,   alarm1 and alarm2 by serial monitor
//  ALARM could be put on pin 2 for LED
//  Serial monitor of LDR values for checking light suitable

unsigned long startMillis;
unsigned long currentMillis;


#include <VarSpeedServo.h>
VarSpeedServo myservo1;  //  pin9    -   bunnies
VarSpeedServo myservo3;  //  pin 10  - me waving

int waves = 0;       //  number of waves from mini me

#include <AFMotor.h>
AF_Stepper motor(48, 2);   //  motor shield port 2

//   IR detectors variables to store the values coming from the LDRs
int LDR1 = 0;  //  A1
int LDR2 = 0;  //  A2

bool alarm1 = LOW;
bool alarm2 = LOW;
bool ALARM  = LOW;
bool bunnypos = LOW;   // LOW = UNDERGROUND

void setup() {
  motor.setSpeed(100);   //  kite drive
  Serial.begin(9600);
}

void loop() {

  sensors();   //  read LDRs

  decide();    //  if out, to go in or vv

  // back to void loop
}

void sensors() {
  LDR1 = analogRead(A1);
  Serial.print("LDR1  ");
  Serial.println(LDR1);
  if (LDR1 < 200) {
    alarm1 = HIGH;
  }
  if (alarm1  == HIGH) {
    Serial.println("Alarm1 HIGH ");
    delay(1000);
  }

  LDR2 = analogRead(A2);
  Serial.print("            LDR2  ");
  Serial.println(LDR2);
  if (LDR2 < 200) {
    alarm2 = HIGH;
  }
  if (alarm2  == HIGH) {
    Serial.println("            Alarm2 HIGH ");
    delay(1000);
  }

  //    this is an exclusive OR
  if ( (alarm1 == HIGH) ^ (alarm2 == HIGH)) {   //   exclusive OR   //   if either high
    ALARM = HIGH;
    Serial.println("                      ALARM is HIGH");
    startMillis = millis();
  }
  else {
    ALARM = LOW;
    if ((alarm1 == HIGH) && (alarm2 == HIGH)) {
      alarm1 = LOW;
      alarm2 = LOW;
    }
  }
    currentMillis = millis();
  if ((currentMillis - startMillis) >= 300000) {  //  5 min
     ALARM = LOW;
      alarm1 = LOW;
      alarm2 = LOW;
      //   reset cos we probably missed exit
  }
}
void decide() {   
    Serial.println("decide");
  if ((bunnypos == HIGH) && (ALARM == HIGH)) {
    scarper();
    wave();      //   Mini me wave
  }
  if ((bunnypos == LOW) && (ALARM == LOW)) { //  IN but safe to come out
    comeOut();
  }
}
void scarper() {
  //  bunnies go down
    Serial.println("scarper");
  myservo1.attach(9);    // Arm servo on pin 9
  myservo1.write(10, 20, true);   //  down fast
  myservo1.detach();
  bunnypos = 0;  //  down burrows
}

void comeOut() {
    Serial.println("come out");
  kite();      // Do kite first   delay before rabbits come up ?
//   kite won't delay detecting the train leaving   :-)  cos it just did
  myservo1.attach(9);                //     bunny servo on pin 9
  myservo1.write(155, 7, true);     //  5 for slowly and 155 is fully up
  myservo1.detach();
  bunnypos = 1;   //  out of burrows

}

void wave() {
  //   Mini me wave
  waves = 0;
    Serial.print("wave  ");
      Serial.println(waves);
  myservo3.attach(10);    // Arm servo on pin 10
  myservo3.write(120, 8, true);     //  start waving
  while (waves < 6) {
    myservo3.write(180, 15, true);     //  CCW
    myservo3.write(120, 15, true);     //  CW
    sensors();    //   check for train leaving between waves
    waves = waves + 1;
  }
  myservo3.write(20, 8, true);     //  relax
  myservo3.detach();
  //       stop waving
}

void kite() {
  //  red kite two revolutions
    Serial.println("kite");
  motor.step(6060, FORWARD, MICROSTEP);
  motor.step(6060, FORWARD, MICROSTEP);
  motor.release();
}
