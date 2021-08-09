//   Bunny Corner -  using  1 servo per rabbit
//   M Beer

//   Hardware running on 5V - switch to disconnect Arduino while on USB power
//   31/07/21   working with three servos
//   Check out time override

//   LDR detectors - EOR section
//   pins A1 and A2 LDRs 
//   pin 10 waving arm  servo motor shield - stepper port #2 kite
//   pin 9 running three servos  -  or more
//   ALARM,   alarm1 and alarm2 monitored by serial monitor
//   ALARM could be put on pin 2 for LED
//   Serial monitor of LDR values for checking light suitable

unsigned long startMillis;
unsigned long currentMillis;
//   If no train clear reset in XXXX millis, clear anyway

#include <VarSpeedServo.h>

VarSpeedServo myservo3;  //  pin 10 PWM  - me waving
int waves = 0;           //  zero count of waves from mini me
VarSpeedServo myservo1;  //  pin9    -   bunnies

#include <AFMotor.h>       //  kite
AF_Stepper motor(48, 2);   //  motor shield port 2

//   light detectors variables to store the values coming from the LDRs
int LDR1 = 0;  //  A1
int LDR2 = 0;  //  A2
bool alarm1 = LOW;
bool alarm2 = LOW;
bool ALARM  = LOW;
bool prevALARM = LOW;  // to detect change of state
bool bunnypos = LOW;   // LOW = UNDERGROUND
bool timing = LOW;    //  millis count not running

void setup() {
  motor.setSpeed(100);      //  kite drive

  Serial.begin(9600);
}

void loop() {

  sensors();   //  read LDRs

  decide();    //  if out, to go in or vv

  delay(500);   //   for testing only

  // back to void loop
}

void sensors() {
  LDR1 = analogRead(A1);
  Serial.print("        LDR1  ");
  Serial.println(LDR1);
  if (LDR1 < 200) {
    alarm1 = HIGH;
  }
  if (alarm1  == HIGH) {
    Serial.print("        Alarm1 ");
    Serial.println(alarm1);
 //   delay(1000);
  }

  LDR2 = analogRead(A2);
  Serial.print("                     LDR2  ");
  Serial.println(LDR2);
  if (LDR2 < 200) {
    alarm2 = HIGH;
  }
  if (alarm2  == HIGH) {
    Serial.print("                     Alarm2  ");
    Serial.println(alarm2);
  //  delay(1000);
  }
     prevALARM = ALARM;   //  to detect a change of ALARM
     
  //    this is an exclusive OR
  if ( (alarm1 == HIGH) ^ (alarm2 == HIGH)) {   //   exclusive OR  //   if either high
      ALARM = HIGH;
      if((ALARM==HIGH)&&(prevALARM==LOW)){
        Serial.print("  ALARM went HIGH ");
        startMillis = millis();
//      must only happen on first change
        Serial.print("  startMillis = ");
        Serial.println(startMillis);
     }
  }
  else {
    ALARM = LOW;
    if ((alarm1 == HIGH) && (alarm2 == HIGH)) {
      alarm1 = LOW;
      alarm2 = LOW;
         Serial.print(" ALARMs reset ");
    }
  }
    currentMillis = millis();
  if ((currentMillis - startMillis) >= 300000) {  //  5 min
     ALARM = LOW;
     alarm1 = LOW;
     alarm2 = LOW;
      //   reset cos we probably missed exit
  }
}     //  ends sensors()

void decide() {   
    Serial.println("decide");
  if ((bunnypos == HIGH) && (ALARM == HIGH)) {
    scarper();
    wave();      //   Mini me wave
  } else {
  if ((bunnypos == LOW) && (ALARM == LOW)) { //  IN but safe to come out
    comeOut();
  }
 }
}

void scarper() {
    Serial.println("scarper");
  //  bunnies go down
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
  myservo1.write(155, 5, true);     //  5 for slowly and 155 is fully up
  myservo1.detach();
  bunnypos = 1;   //  out of burrows

}

void wave() {
  //   Mini me wave
  waves = 0;
    Serial.print("wave  ");
      Serial.println(waves);
  myservo3.attach(10);    // Arm servo on pin 10
  myservo3.write(120, 20, true);     //  raise arm
  while (waves < 6) {               //  six waves
    myservo3.write(180, 15, true);     //  CCW
    delay (500);
    myservo3.write(120, 15, true);     //  CW
    sensors();    //   check for train leaving between waves
    waves = waves + 1;
  }   //                            //  end while
  myservo3.write(20, 15, true);     //  relax
  myservo3.detach();

  //       stop waving
}

void kite() {
  //  red kite one revolution
    Serial.println("kite");
  motor.step(6060, FORWARD, MICROSTEP);
  motor.release();
}
