//  servo operated level crossing gates  stepper raising bridge
//  code   MJB    -   
//  using proven code for bridge    07/01/21
//  ***   confusion between motors
//  syntax for stepper updated to motor2.stepper  09/02/21

//   serial monitoring
//  Motor1 and Motor2 used for LEDs

//  using Arduino with LM293 motor shield driving Nema17 stepper  and Motor#1 for signal relay
//  using MOTOR 1 to control signal  Motor 2 for alt flashing lights
//  9 & 10 for servos,  leaves    2 & 3 spare, and 13 for speaker
//  gates based on  Railway Modeller December 2018
//  NOTE servos must rotate in opposite directions    /     swapped parameters for second gate
//   switch for gates (A2) and bridge (A4)  and   (A0) for down microswitch,

#include <VarSpeedServo.h>
VarSpeedServo myservo1;
VarSpeedServo myservo2;

#include <AFMotor.h>    //   stepper + LM293 + 2 servos
//  const int stepsPerRevolution = 200;    // for Nema 17
// connect stepper motor to port #2 (M3 and M4)
// signal on Motor 1  and nerner lights on motor 2

AF_Stepper motor2(200, 2);

int gatepos = 0;   // 2 for road  -  1  for trains
int beep = 0;
int gatebutton = 1;

int bridgepos = 0; // 0 =  down   UP = 1
int switchread = 1023;
int limitswitch = 1023;
int bridgebutton = 1;
int bridgecheck = 1;  //  do check first run
int closed = 1;

//AF_DCMotor motor1(1);
 //  AF_DCMotor motor(1, MOTOR12_64KHZ);   //  set for signal
//AF_DCMotor motor2(2);
//   AF_DCMotor motor(2,MOTOR12_64KHZ);    //  nerner lights

void setup() {

  //    flashing RED LEDs   //   on motor 2
  //    Signal Reg or Green //   on motor 1

  pinMode(13, OUTPUT); //    speaker     pin 13 to GND  8ohm speaker
  pinMode(A0, INPUT); //     A0   bridge down button or contact
  pinMode(A2, INPUT); //     bridge button
  pinMode(A4, INPUT); //     gates

 // AF_DCMotor1(1);      //  set for signal
 // motor1.setSpeed(255);
 // motor1.run(RELEASE);

//  AF_DCMotor2(2);    //  nerner lights
//  motor2.setSpeed(0);
//  motor2.run(RELEASE);

  // set up Serial library at 9600 bps
  Serial.begin(9600);
  //    delay(2000); while (!Serial);
  Serial.println("Run programme");
}

void loop () {

//  if (bridgecheck > 0) { // hasn't been initialised
//    bridgeInit();
//  }
//  if (gatepos == 0) {    // hasn't been initialised
//    gatesInit();
//  }

  //   this is the sequence

  if (gatepos == 2) { // to road ? //   only option
    gates2rail();
  }

  if ( gatepos == 1) { //  TO RAIL  two options
    gates2road();
    bridgeup();
    //   both look for own button
  }

  if (bridgepos == 1){ //  up
    bridgedown();   //   only option
}

if (bridgepos == 0) { //  its down  //  two options  
  gates2road();
  bridgeup();
  //   both look for own button
}

}  //  loop back


void bridgeInit() {
  //  Initialise bridge Down        //   from station initialise door position routine

  Serial.println("bridgeInit");
  switchread = (analogRead(A0));   //  1023 if open
    //  is bridge down ?
  if (switchread > 200) {
    closed = 0;     //  not closed
    //  so drive down
  }
  while ( closed == 0) {
    motor2.step(10, BACKWARD, MICROSTEP);    //  bridge down a bit
    switchread = (analogRead(A0));
    if (switchread < 10) { //   microswitch closed
      closed = 1;
      motor2.release();
      bridgecheck = 0;
    }
  }
}  //   end bridge init

void gatesInit() {
  //      initialise gate servos to  ROAD first time around             
  Serial.println("gatesInit");
    myservo1.attach(9);             //  down gate
    myservo1.write (135, 5, true);  // to across track
    myservo1.detach();
    myservo2.attach(10);            //  up gate
    myservo2.write (45, 5, true);   //   to across track
    myservo2.detach();
    gatepos = 2;                    //   gates favour road
}  //   end gates init

void   gates2rail(){
  Serial.println("gates2rail");
  gatebutton = analogRead(A4);
  if (gatebutton == LOW) {         //  for trains   //  road closing
//      motor1.run(FORWARD);    //  PLUS polarity to signal GREEN
//      motor1.setSpeed(255);   // max glim

//         Preamble flashing and sounds
      beep = 0;
      while ( beep < 20) {
//        motor2.run(FORWARD);    //  One or other RED on
//        motor2.setSpeed(255);   // max glim
        tone(13, 1100, 300);   //  high beep
        delay(300);
//        motor2.run(BACKWARD);    //   other RED on
        tone(13, 900, 300);    //  low beep
        delay(300);
        beep = beep + 1;
      }
    }
  //  Move gates
  myservo1.attach(9);              // first servo on pin 9
  myservo1.write(129, 4, true);    // sends first gate to ROAD CLOSED
  myservo1.write(122, 12, true);   // opens gates slightly to start bounce sequence
  myservo1.write(129, 20, true);   // close
  myservo1.write(125, 20, true);   // open bit less
  myservo1.write(129, 20, true);   // close
  myservo1.write(127, 20, true);   // open bit less
  myservo1.write(129, 20, true);   // final close
  myservo1.detach();
  //  second gate
  myservo2.attach(10);
  myservo2.write(37, 4, true);   // sends second gate to Road CLOSED
  myservo2.write(34, 12, true);  // open slightly to start bounce seq
  myservo2.write(37, 20, true);  // closes
  myservo2.write(35, 20, true);  // open bit less
  myservo2.write(37, 20, true);  // closes
  myservo2.write(36, 20, true);  // open bit less
  myservo2.write(37, 20, true);  // close finally
  myservo2.detach();
  gatepos = 1;
  // gates are set to RAIL  -  ok to raise bridge
}


void  bridgeup() {
  Serial.println("bridgeup");
    bridgebutton = analogRead(A2);    //   bridge button pressed
    if (bridgebutton > 10) {          //  then  lift  bridge
      beep = 0;
      while ( beep < 10) {            //   ten beeps before moving
        tone(13, 1100, 300);
        delay(300);
        tone(13, 900, 300);
        delay(300);
        beep = beep + 1;
      }
    }
    motor2.step(1500, FORWARD, MICROSTEP);  //  bridge drives up    -   assume FORWARD is raise
    bridgepos = 1;                            //   it's up now
  }      //   end bridgeup button

void  bridgedown() {
  Serial.println("bridgedown");
  //      READING  DOWN LIMIT SWITCH   (A0)
    bridgebutton = analogRead(A2);   //  want to go down?
    if (bridgebutton > 10) {                   //  then   lower bridge
      beep = 0;
      while ( beep < 10) {                   //   ten beeps before moving
        tone(13, 900, 300);
        delay(300);
        beep = beep + 1;
      }
     
       Serial.println("Microstep steps backwards and read limit switch");
    limitswitch = analogRead (A0);
    while (limitswitch > 500) {
      motor2.step(10, BACKWARD, MICROSTEP);
      limitswitch = analogRead (A0);
    }
    Serial.println("backward done -  limit switch hit");
             closed = 1;
    motor2.release();
          bridgepos = 0;  //   END OF BRIDGEdown button
    }
  }   //   END OF BRIDGEDOWN

void gates2road() {
  Serial.println("gates2road");
  gatebutton = analogRead(A4);//  IS GATEBUTTON PRESSED FOR ROAD
  if ((gatebutton < 500) && (bridgepos = 0)) {   //  bridge must be down
    //   start opening gates to road
    //  RED for trains
 //   motor1.run(BACKWARD);    //  MINUS polarity to signal  RED
    // ?     motor1.setSpeed(255);   // max glim - ALWAYS
    //   second gate first
    myservo2.attach(10);             // second servo  pin10
    myservo2.write(119, 4, true);    // sends second gate to Rail CLOSED
    myservo2.write(112, 12, true);   // opens gates slightly to start bounce sequence
    myservo2.write(119, 20, true);   // closes gates
    myservo2.write(115, 20, true);   // open bit less
    myservo2.write(119, 20, true);   // close
    myservo2.write(117, 20, true);   // open bit less
    myservo2.write(119, 20, true);   // final close
    myservo2.detach();

    myservo1.attach(9);             // first servo on pin 9
    myservo1.write(47, 4, true);    // sends first gate to RAIL CLOSED
    myservo1.write(52, 12, true);   // open slightly to start bounce seq
    myservo1.write(47, 20, true);   // close
    myservo1.write(51, 20, true);   // open bit less
    myservo1.write(47, 20, true);   // close
    myservo1.write(49, 20, true);   // open bit less
    myservo1.write(47, 20, true);   // close finally
    myservo1.detach();
 //   motor2.run(0);
 //   motor2.run(RELEASE);   //  REDS OFF
    gatepos = 2;
  }
}   //  end gates2road
