//  Bridge software SERIAL MONITORING reinstated 

//  WORKING   MJB  03/03/21
//  Bridge and gates working correctly
//  gate bounce removed pro tem
//  using Arduino with LM293 motor shield driving Nema17 stepper 
//  9 & 10 for servos,   2 for warning beacon LEDs
//  DCmotor1 and DCmotor2 used for LEDs

#include <VarSpeedServo.h>
VarSpeedServo myservo1;
VarSpeedServo myservo2;

#include <AFMotor.h>    //   stepper + LM293 + 2 servos
AF_Stepper stepmotor(200, 2);

int gatepos = 0;   // 2 for road  -  1  for trains
int beep = 0;  //  controls flash rate
int gatebutton = 1;
int gatecheck = 1;    //   init to road

int bridgepos = 0; // 0 =  down   UP = 1
int switchread = 1023;
int limitswitch = 1023;
int bridgebutton = 1023;
int bridgecheck = 1;  //  do check first run

// LED CODE
   AF_DCMotor DCmotor1(1);    //    set for signal
   AF_DCMotor DCmotor2(2);   //     nerner lights

void setup() {

  pinMode(2, OUTPUT);  //    beacon LEDs on bridge  
  pinMode(A0, INPUT); //     bridge down contact
  pinMode(A2, INPUT); //     bridge button
  pinMode(A4, INPUT); //     gates button
       
  DCmotor1.setSpeed(255);   // max glim
  DCmotor2.setSpeed(255);   // max glim

  stepmotor.setSpeed(10);

    Serial.begin(9600);
}

void loop () {

  if (bridgecheck > 0) { // hasn't been initialised
    bridgeInit();
  }

    if (gatepos == 0) {    // hasn't been initialised
      gatesInit();
    }

  //   this is the sequence

  if (gatepos == 2) { // RAIL - MOVE to road ? -  only option
    gates2rail();
  }

  if( ( gatepos == 1)&&(bridgepos == 0)) { //  TO RAIL -  two options
    gates2road();
    bridgeup();
  }

  if (bridgepos == 1) { //  up
    bridgedown();   //   only option
  }
}  //  loop back


void bridgeInit() {  //   bridge - Down 
    Serial.println("bridge init");      
      beep = 0;
      while ( beep < 10) {       //   ten beacons before moving
      digitalWrite(2, HIGH);
      delay(200);
      digitalWrite(2, LOW);
      delay(500);
      beep = beep + 1;
    }
  switchread = (analogRead(A0));  
  if (switchread > 200) {
        bridgepos = 1;     //  not close

  }
  while ( bridgepos > 0) {
    stepmotor.step(10, BACKWARD, MICROSTEP);    //  bridge down a bit
    switchread = (analogRead(A0));
    if (switchread < 10) { //   limit switch closed
        bridgepos = 0;   
    }
  }
  stepmotor.release();
      bridgecheck = 0; 
      DCmotor2.run(RELEASE); 
}  //   end bridge init

void gatesInit() {      //   to  ROAD first time around
      Serial.println("gates  init"); 
  if (gatecheck > 0){
    myservo1.attach(9);             //  ? gate
    myservo1.write (35, 5, true);  // to across track ?
    myservo1.detach();
    myservo2.attach(10);            //  up gate
    myservo2.write (35, 5, true);   //   to across track ?
    myservo2.detach();
    gatepos = 2;                    //   gates favour road
      DCmotor1.setSpeed(255);   // max glim
      DCmotor1.run(FORWARD);    //  SIGNAL RED 
          Serial.println("signal red");       
      gatecheck = 0;
  }
}  //   end gates init

void   gates2rail() {
    gatebutton = analogRead(A4);
    if (gatebutton < 500) {         //  for trains   //  road closing  
    Serial.println("move gates to rail"); 
      beep = 0;
      while ( beep < 20) {
      DCmotor2.setSpeed(255);   // max glim
      DCmotor2.run(FORWARD);    //  One or other RED on
      delay(300);
      DCmotor2.run(BACKWARD);    //   other RED on
      delay(300);
      beep = beep + 1;
    }
  //  Move gates
    myservo1.attach(10);            
    myservo1.write(135, 5, true);   //  to rail
    myservo1.detach();
//  second gate
    myservo2.attach(9);
    myservo2.write(135, 5, true);  // to rail
    myservo2.detach();
    gatepos = 1;
    DCmotor1.setSpeed(255);   // max glim     
    DCmotor1.run(BACKWARD);    //  PLUS polarity to signal GREEN
        Serial.println("gates to rail signal green"); 
}
}

void  bridgeup() {
   bridgebutton = analogRead(A2);
   if (bridgebutton < 500) {      //   bridge button -  lift  bridge
    DCmotor1.setSpeed(255);   // max glim     
    DCmotor1.run(FORWARD);    // Signal RED
        Serial.println("signal red"); 
      beep = 0;
      while ( beep < 20) {       //   ten flashes before moving
      digitalWrite(2, HIGH);
      delay(20);
      digitalWrite(2, LOW);
      delay(500);
      beep = beep + 1;
    }
    stepmotor.step(1500, FORWARD, MICROSTEP);  //  bridge drives up
    stepmotor.release ();
    bridgepos = 1;
  }
}      //   end bridgeup

void  bridgedown() {
  bridgebutton = analogRead(A2);   //  want to go down?
  if (bridgebutton < 10) {                   //  then   lower bridge
    beep = 0;
    while ( beep < 20) {         //   ten beacons before moving
      digitalWrite(2, HIGH);
      delay(200);
      digitalWrite(2, LOW);
      delay(500);
      beep = beep + 1;
    }
    limitswitch = analogRead (A0);
    while (limitswitch > 500) {
      stepmotor.step(10, BACKWARD, MICROSTEP);
      limitswitch = analogRead (A0);
    }
    stepmotor.release();
    bridgepos = 0; 
    DCmotor1.setSpeed(255);   //   max glim     
    DCmotor1.run(BACKWARD);    //   signal GREEN
        Serial.println("signal green"); 
  }
}   //   END OF BRIDGEDOWN

void gates2road() {
  gatebutton = analogRead(A4);//  IS GATEBUTTON PRESSED FOR ROAD?
    if (gatebutton < 500){
    //   start opening gates to road
        Serial.println("gates to road"); 
    DCmotor1.setSpeed(255);   
    DCmotor1.run(FORWARD);    //   signal  RED
    Serial.println("signal red"); 
    //   second gate first
    myservo2.attach(9);             
    myservo2.write(35, 5, true);   //  to road
    myservo2.detach();

    myservo1.attach(10);            
    myservo1.write(35, 5, true);   // to road
    myservo1.detach();
//  gates moved
    DCmotor2.run(RELEASE);   //  FLASHING REDS OFF
    gatepos = 2;
        Serial.println("gates to road stop flashing red"); 
  }
}   //  end gates2road
