//  GGR SOFTWARE repin
//  from Grandads Garden Select 28 July
//   software looked OK     16/08/25
//  WAS
//  Points change screws up traction function
//  Chat GPT says it's servo clash with Timers
//  second Pin reassingment version   08/09/25 - fix
//  PWM on pin 5 - timer 0
//  Chat GPT fixes again by losing "true" in servo calls.   :-)


//  pin 2   Signal 1 green                                
//p  pin 3  Signal 2 green           Timer 2   
//  pin 4   Whistle                               
//p  pin 5   PMW out                 Timer 0     
//p  pin 6   spare                   Timer 0      
//  Pin 7   E STOP LED                         
//  pin 8   see 12 below
//p  pin 9   Servo1                  Timer 1
//p  pin 10  Servo2                  Timer 1
//p  Pin 11  Remote Rec input        Timer 2
//       IR Rcv  [X]  pins l-3 -> code rec , GND, vcc
//  pin 12  and pin 8 Relays combined for ESTOP and CW / CCW


#include "IRremote.h"

IRrecv My_Receiver(11);
decode_results signals;
unsigned long ircodereceived;

#include <VarSpeedServo.h>
VarSpeedServo myservo1;  //  pin9     Point1
VarSpeedServo myservo2;  //  pin10     Point2

bool CCW = LOW;
bool Estop = HIGH;    //   start with drive off
bool EstopRS = LOW;
int count = 0;   //   cycle time //  for train accel
int motorValue5 = 0;
int speedReq = 0;
int speedNow = 0;
int Step = 0;
bool Point11 = LOW;
bool Point12 = LOW;
bool Point21 = LOW;
bool Point22 = LOW;
bool blowWhistle = LOW;
bool HoldOff = LOW;
int  servospeed = 10;  //  set for whole sketch


IRrecv irrecv (11);
unsigned long ircodereceiveed;   //  IR code received

//   "Prototypes" because compiler missed them at the end
void EMGstop();
void RESET();
void Calc();
void Whistle();

void setup() {
  Serial.begin(9600);
  delay(2000);
  irrecv.enableIRIn();  //  enable input from IR receiver
  if (irrecv.decode(&signals)) {
    ircodereceived = signals.value;
    Serial.println(ircodereceived);
  }
  irrecv.resume();  //  get next signal
  switch (ircodereceived) {
  }

  pinMode(2, OUTPUT);   //  Sig 1
  pinMode(3, OUTPUT);   //  SIG 2
  pinMode(4, OUTPUT);   //  Whistle
  pinMode(5, OUTPUT);   //  PWM
  pinMode(6, OUTPUT);   //  spare
  pinMode(7, OUTPUT);   //  ESTOP warning LED
  pinMode(9, OUTPUT);   //  servo1
  pinMode(10, OUTPUT);  //  servo2
  //  pin 11                IR code input
  pinMode(8, OUTPUT);   //  reverse relay    * combined with 12
  pinMode(12, OUTPUT);  //  Estop Relay      * combined with 8
                 //  both together for direction Different for Emg stop

  //
  // set PWM freq for pin 9
  TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 divisor to     1 for PWM frequency of 31372.55 Hz
 
    myservo1.attach(9);
    myservo2.attach(10);
}

void loop() {
  if (Estop == HIGH) {
    EMGstop();
  }
  Read();      //   IR commands   and switch
  Calc();      //   train speed
  if (EstopRS == HIGH) {
    RESET();
  }
  if (blowWhistle == HIGH) {
    Whistle();
  }
  Points();    //   Set points
  Signals();   //   LEDs set from points and direction
}  //   void loop


void Read() {   //    includes switch
  //  Reset Flags
  Estop = LOW;
  EstopRS = LOW;
  blowWhistle = LOW;

  //  READ REMOTE
  if (irrecv.decode(&signals)) {
    ircodereceived = signals.value;
    irrecv.resume(); //  get next signal
    Serial.println(ircodereceived);
    switch (ircodereceived) {

      case  2553984193: //   (ring) left arrow - CCW
        CCW = HIGH;
        digitalWrite(8, HIGH);  //   Reverse  on
        digitalWrite(12, HIGH); //   Reverse  on
        break;

      case 553866289:     //      Right arrow  CW
        CCW = LOW;
        digitalWrite(8, LOW);     //  Reverse  off
        digitalWrite(12, LOW);    //  Reverse  off
        break;

      case 3255005345: //     Up arrow    speed up
        Step = 1;
        break;

      case 2173897225: //     Down arrow  slow down
        Step = -1;
        break;

      case 3138312673: //     OK  -  hold speed
        Step = 0;
        break;

      case 4129172055: //  red Power   = E STOP
        Estop = HIGH;
        break;

      //   - start session with Estop ON and press VIERA to reset

      case 4188329544: //  VIERA button = RESET
        EstopRS = HIGH;
        break;

      //   POINTS

      case 604803425: //   RED  Point 1 straight  servo pin 9
        Point11 = HIGH;
        Point12 = LOW;
        break;

      case 2724465097: //   GREEN  Point 1 curved servo pin 9
        Point12 = HIGH;
        Point11 = LOW;
        break;

      case 3760167553:  //   YELLOW  Point 2 straight servo pin 10
        Point21 = HIGH;
        Point22 = LOW;
        break;

      case 1760049649: //   BLUE  Point 2 curved servo pin 10
        Point22 = HIGH;
        Point21 = LOW;
        break;

      case 2182043242: //      Whistle    "GUIDE" button
        blowWhistle = HIGH;
        break;

    }    //   end of switch
  }    //   end of proc
}    //   end of Read()

void Points() {

  if (Point11 == HIGH) {   // RED button
    myservo1.write(45);
  }
  if (Point12 == HIGH) {     // GREEN
    myservo1.write(135);
  }
  if (Point21 == HIGH) {  //  YELLOW
    myservo2.write(45);
  }
  if (Point22 == HIGH) {   //   BLUE
    myservo2.write(135);
  }
} //   end points

void Signals() {
  //  Signal 1
  if (( CCW == HIGH) && (Point11 == HIGH && Point12 == LOW)) {
    digitalWrite(2, HIGH);  // s1  GREEN ON
  }
  else {
     digitalWrite(2, LOW);  // s1 GREEN OFF
  }

  //    Signal 2
  if (( CCW == LOW) && (Point21 == HIGH && Point22 == LOW)) {
    digitalWrite(3, HIGH);  //    s2 GREEN ON
  }
  else {
    digitalWrite(3, LOW);  //     s2 GREEN OFF
  }

  //   Whistle
  if (blowWhistle == HIGH) {
    Whistle();
    blowWhistle = LOW;
  }
}  //   end signals

void Calc() {
  if (HoldOff == LOW){

  //  Slow down 500 times.
  count = count + 1;
  if (count > 500) {         //    overall train speed var control
    //  calculate and drive routine //  Add set amount + or - or zero to current speed
    speedNow = motorValue5;// take in current speed
    //   update speed with Step   from switch ( +, or -, or 0)
    speedReq = speedNow + Step;
    // speedReq must be between 0 and 255
    if (speedReq < 0) {
      speedReq = 0;
    }
    if (speedReq > 255 ) {
      speedReq = 255;
    }
    motorValue5 = speedReq;   //    output speed
    Serial.print("motor value5  ");
    Serial.println(motorValue5);
    analogWrite(5, motorValue5);
    count = 0 ;
  }
  }
}

void Whistle() {    //     temporary simple
  Serial.println ("  PEEP");
  int frequency = 3000;  // Target frequency in Hz
  unsigned long duration = 100000;  // Duration in microseconds (e.g., 50 ms)
  int period = 1000000 / frequency;  // Period in microseconds
  int pulse = period / 2;            // Half period = high or low pulse width
  unsigned long startTime = micros();

  while ((micros() - startTime) < duration) {
    digitalWrite(4, HIGH);
    delayMicroseconds(pulse);//  High half cycle
    digitalWrite(4, LOW);
    delayMicroseconds(pulse);//  Low half cycle
  }
}  //   end whistle


void EMGstop() {
  Serial.println(" EMGstop");
  digitalWrite(7, HIGH);  //   ESTOP LED ON   Blue
  HoldOff = HIGH;
  digitalWrite(12, HIGH);   //  power cut from rails
  digitalWrite(8, LOW);     //  with above
  analogWrite(5, 0);
  Step = 0;
  motorValue5 = 0 ;
  Estop = LOW;
}

void RESET() {
  Serial.println(" Reset Estop");
  digitalWrite(12, LOW);   //  Power restored (Forward)
  digitalWrite(8, LOW);    //  with above
  digitalWrite(7, LOW);  //   ESTOP LED OFF
  HoldOff = LOW;
  EstopRS = LOW;
}
