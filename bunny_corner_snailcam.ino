//   Bunny Corner   Snailcam
//   M Beer
//   BUNNY CORNER     01/06/21     COMPILES
//   DC motor driving rabbits - with snail cam

//   ??  TO DO   modify decide() with time override
//   If no train clear reset in XXXX millis, clear anyway
unsigned long startMillis;
unsigned long currentMillis;

//   LDR detectors - EOR section

//  pins A1 and A2 LDRs -  A4 reads Limit Switch
//  pin 10 waving arm  servo motor shield - stepper port #2 kite
//  bunny drive DCmotor1
//  ALARM,   alarm1 and alarm2 by serial monitor
//  ALARM could be put on pin 2 for LED
//  Serial monitor of LDR values for checking light suitable

#include <VarSpeedServo.h>

VarSpeedServo myservo3;  //  pin 10 PWM  - me waving
int waves = 0;           //  zero count of waves from mini me

#include <AFMotor.h>       //  kite
AF_Stepper motor(48, 2);   //  motor shield port 2

//   IR detectors variables to store the values coming from the LDRs
int LDR1 = 0;  //  A1
int LDR2 = 0;  //  A2
int limitSw = 0;       //   A4  Microswitch - made for out = LOW
                       // Come out - drive to cam lobe against spring
                       // Go in - drop over lobe of cam and spring in
                       // Read on A4
bool alarm1 = LOW;
bool alarm2 = LOW;
bool ALARM  = LOW;
bool bunnypos = LOW;   // LOW = UNDERGROUND
bool timing = LOW;    //  millis count not running

AF_DCMotor DCmotor1(1);    //    rabbit driver

void setup() {
  motor.setSpeed(100);      //  kite drive
  DCmotor1.setSpeed(255);   //  rabbit drive max speed
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

  //    this is an exclusive OR
  if ( (alarm1 == HIGH) ^ (alarm2 == HIGH)) {   //   exclusive OR   //   if either high
    ALARM = HIGH;
    Serial.print("                             ALARM  ");
    Serial.println(ALARM);
    startMillis = millis();
  }
  else {
    ALARM = LOW;
    if ((alarm1 == HIGH) && (alarm2 == HIGH)) {
      alarm1 = LOW;
      alarm2 = LOW;
         Serial.print("                             ALARM  ");
    Serial.println(ALARM);
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
    Serial.println("scarper");
   DCmotor1.setSpeed(255);
    limitSw = 1024;         //  microswitch OC (not pressed)
    while (limitSw > 200) {
      DCmotor1.run(FORWARD);
      limitSw = (analogRead(A4));
//               (limitSw < 200) means   microswitch closed
  }  //   end while
    Serial.print (limitSw);
    DCmotor1.run(RELEASE);
  bunnypos = 0;  //  down burrows
}

void comeOut() {
    Serial.println("come out");
  kite();      // Do kite first   delay before rabbits come up ?
//   kite won't delay detecting the train leaving   :-)  cos it just did
    limitSw = 0;
    DCmotor1.setSpeed(255);
    while (limitSw < 200 ){         //   Mswitch pressed . closed
      DCmotor1.run(FORWARD);        //   off the lobe of the cam
      limitSw = (analogRead(A4));
//             limitSw > 200 means  //   Microswitch open
      }   //                        //   end while
    Serial.print (limitSw);
    DCmotor1.run(RELEASE);
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
