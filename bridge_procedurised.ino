//  servo operated level crossing gates  stepper raising bridge
//  code   MJB  

//  Bridge and gates working correctly
//  DCmotor1 and DCmotor2 used for LEDs  Not Working
//   19/01/21

//  using Arduino with LM293 motor shield driving Nema17 stepper  and Motor#1 for signal relay
//  9 & 10 for servos,     13 for speaker
//  gates bounce -  Railway Modeller December 2018
//  NOTE servos must rotate in opposite directions    /     swapped parameters for second gate
//   switch for gates (A2) and bridge (A4)  and (A0) for down limitswitch,

#include <VarSpeedServo.h>
VarSpeedServo myservo1;
VarSpeedServo myservo2;

#include <AFMotor.h>    //   stepper + LM293 + 2 servos
//  const int stepsPerRevolution = 200;    // for Nema 17
// connect stepper motor to port #2 (M3 and M4)
// signal on Motor 1  and nerner lights on motor 2

AF_Stepper stepmotor(200, 2);

int gatepos = 0;   // 2 for road  -  1  for trains
int beep = 0;
int gatebutton = 1;
int gatecheck = 1;    //   init to road

int bridgepos = 0; // 0 =  down   UP = 1
int switchread = 1023;
int limitswitch = 1023;
int bridgebutton = 1023;
int bridgecheck = 1;  //  do check first run
int closed = 1;

AF_DCMotor DCmotor1(1);    //    set for signal
AF_DCMotor DCmotor2(2);   //     nerner lights

void setup() {

  pinMode(13, OUTPUT); //    speaker     pin 13 to GND  8ohm speaker
//pinMode(2, OUTPUT);  //    orange LEDs on bridge  
  pinMode(A0, INPUT); //     A0   bridge down button or contact
  pinMode(A2, INPUT); //     bridge button
  pinMode(A4, INPUT); //     gates

      DCmotor1.setSpeed(255);   // max glim
     DCmotor2.setSpeed(255);   // max glim

  stepmotor.setSpeed(10);

  // set up Serial library at 9600 bps
  Serial.begin(9600);
  //    delay(2000); while (!Serial);
  Serial.println("Run programme");
}

void loop () {

  if (bridgecheck > 0) { // hasn't been initialised
    bridgeInit();
  }

    if (gatepos == 0) {    // hasn't been initialised
      gatesInit();
    }

  //   this is the sequence

  if (gatepos == 2) { // RAIL  MOVE to road ? //   only option
    Serial.println("ROAD - move gates to rail ?");
    gates2rail();
  }
delay(2000);
  if( ( gatepos == 1)&&(bridgepos == 0)) { //  TO RAIL  two options
    Serial.println("RAIL   Move gates to road ?  or Bridge up ?");
    gates2road();
    bridgeup();
  }
delay(2000);
  if (bridgepos == 1) { //  up
    Serial.println("Move Bridge down ?");
    bridgedown();   //   only option
  }

  //  UPDATE
  
  if (bridgepos == 1) {
    Serial.print("BRIDGE is = up ");
  }
   if (bridgepos == 0) {
    Serial.print("BRIDGE is down");
  }
    if (gatepos == 0){
         Serial.println("                Gate is to ROAD");
    }
        if (gatepos == 1){
         Serial.println("                Gate is to RAIL");
    }

delay(2000);
}  //  loop back


void bridgeInit() {
  //  Initialise bridge - Down       
  //  is bridge down ?
      beep = 0;
    while ( beep < 5) {
      DCmotor2.run(FORWARD);    //  One or other RED on
//      DCmotor2.setSpeed(255);   // max glim
      digitalWrite(2, HIGH);
      tone(13, 1100, 300);   //  high beep
      delay(300);
      DCmotor2.run(BACKWARD);    //   other RED on
      digitalWrite(2, LOW);      
      tone(13, 900, 300);    //  low beep
      delay(300);
      beep = beep + 1;
    }
  switchread = (analogRead(A0));   //  1023 if open
  if (switchread > 200) {
    bridgepos = 1;     //  not closed
    //  so drive down

  }
  while ( bridgepos > 0) {
    stepmotor.step(10, BACKWARD, MICROSTEP);    //  bridge down a bit
    switchread = (analogRead(A0));
    if (switchread < 10) { //   limit switch closed
        bridgepos = 0; 
        Serial.println("bridge Initialised");     
    }
  }
  stepmotor.release();
      bridgecheck = 0;  
}  //   end bridge init

void gatesInit() {
  //      initialise gate servos to  ROAD first time around
  if (gatecheck > 0){
  myservo1.attach(9);             //  down gate
  myservo1.write (135, 5, true);  // to across track
  myservo1.detach();
  myservo2.attach(10);            //  up gate
  myservo2.write (45, 5, true);   //   to across track
  myservo2.detach();
  gatepos = 2;                    //   gates favour road
      Serial.println("                                Gates initialised to ROAD");
      DCmotor2.setSpeed(255);   // max glim
      DCmotor1.run(BACKWARD);    //  SIGNAL RED 
  gatecheck = 0;
      Serial.println(" SIGNAL RED");
  }
}  //   end gates init

void   gates2rail() {
  Serial.println("gates2rail ?");
  gatebutton = analogRead(A4);
  if (gatebutton < 500) {         //  for trains   //  road closing
       Serial.print("gatebutton  ");
       Serial.println(gatebutton);
       Serial.println("moving gates2rail");   
    //         Preamble flashing and sounds
    beep = 0;
    while ( beep < 20) {
      DCmotor2.setSpeed(255);   // max glim
      DCmotor2.run(FORWARD);    //  One or other RED on

      tone(13, 1100, 300);   //  high beep
      delay(300);
      DCmotor2.run(BACKWARD);    //   other RED on
      tone(13, 900, 300);    //  low beep
      delay(300);
      beep = beep + 1;
    }
  //  Move gates
  myservo1.attach(10);              // first servo on pin 9
  myservo1.write(129, 4, true);    // sends first gate to ROAD CLOSED
  myservo1.write(122, 12, true);   // opens gates slightly to start bounce sequence
  myservo1.write(129, 20, true);   // close
  myservo1.write(125, 20, true);   // open bit less
  myservo1.write(129, 20, true);   // close
  myservo1.write(127, 20, true);   // open bit less
  myservo1.write(129, 20, true);   // final close
  myservo1.detach();
  //  second gate
  myservo2.attach(9);
  myservo2.write(37, 4, true);   // sends second gate to Road CLOSED
  myservo2.write(34, 12, true);  // open slightly to start bounce seq
  myservo2.write(37, 20, true);  // closes
  myservo2.write(35, 20, true);  // open bit less
  myservo2.write(37, 20, true);  // closes
  myservo2.write(36, 20, true);  // open bit less
  myservo2.write(37, 20, true);  // close finally
  myservo2.detach();
  gatepos = 1;
      Serial.println("Gates now to RAIL");
    DCmotor1.setSpeed(255);   // max glim     
    DCmotor1.run(FORWARD);    //  PLUS polarity to signal GREEN
     Serial.println("SIGNAL GREEN");
}
}

void  bridgeup() {

  bridgebutton = analogRead(A2);
  Serial.println(bridgebutton);
  if (bridgebutton < 500) {      //   bridge button -  lift  bridge
    Serial.println("bridge moving up");
    beep = 0;
    while ( beep < 10) {                   //   ten beeps before moving
      tone(13, 900, 300);
      digitalWrite(2, HIGH);
      delay(1000);
      digitalWrite(2, LOW);
      beep = beep + 1;
    }
    stepmotor.step(500, FORWARD, MICROSTEP);  //  bridge drives up

    stepmotor.release ();
    bridgepos = 1;
    Serial.println("bridge is now UP");
  }
}      //   end bridgeup

void  bridgedown() {
  Serial.println("bridge down ?");
  bridgebutton = analogRead(A2);   //  want to go down?
  Serial.println(bridgebutton);
  if (bridgebutton < 10) {                   //  then   lower bridge
    beep = 0;
    while ( beep < 10) {                   //   ten beeps before moving
      tone(13, 900, 300);
      digitalWrite(2, HIGH);
      delay(1000);
      digitalWrite(2, LOW);
      beep = beep + 1;
    }
    Serial.println(" read limit switch");
    limitswitch = analogRead (A0);
    while (limitswitch > 500) {
      stepmotor.step(10, BACKWARD, MICROSTEP);
      limitswitch = analogRead (A0);
    }
    stepmotor.release();
    Serial.println(" limit switch hit");
    bridgepos = 0; 
        Serial.println("Bridge is now DOWN");    
        DCmotor1.setSpeed(255);   // max glim     
    DCmotor1.run(FORWARD);    //  PLUS polarity to signal GREEN
     Serial.println("SIGNAL GREEN");
  }
}   //   END OF BRIDGEDOWN



void gates2road() {
  Serial.println("gates2road ?");
  gatebutton = analogRead(A4);//  IS GATEBUTTON PRESSED FOR ROAD
  Serial.println(gatebutton);
    if (gatebutton < 500){
    //   start opening gates to road
    //  RED for trains
    DCmotor1.setSpeed(255);   // max glim - ALWAYS
    DCmotor1.run(BACKWARD);    //  MINUS polarity to signal  RED

    //   second gate first
    myservo2.attach(9);             // second servo  pin10
    myservo2.write(119, 4, true);    // sends second gate to Rail CLOSED
    myservo2.write(112, 12, true);   // opens gates slightly to start bounce sequence
    myservo2.write(119, 20, true);   // closes gates
    myservo2.write(115, 20, true);   // open bit less
    myservo2.write(119, 20, true);   // close
    myservo2.write(117, 20, true);   // open bit less
    myservo2.write(119, 20, true);   // final close
    myservo2.detach();

    myservo1.attach(10);             // first servo on pin 9
    myservo1.write(47, 4, true);    // sends first gate to RAIL CLOSED
    myservo1.write(52, 12, true);   // open slightly to start bounce seq
    myservo1.write(47, 20, true);   // close
    myservo1.write(51, 20, true);   // open bit less
    myservo1.write(47, 20, true);   // close
    myservo1.write(49, 20, true);   // open bit less
    myservo1.write(47, 20, true);   // close finally
    myservo1.detach();
//  gates moved
//    DCmotor2.run(0);
    DCmotor2.run(RELEASE);   //  FLASHING REDS OFF
    gatepos = 2;
        Serial.println("Gates now to ROAD");
  }
}   //  end gates2road
