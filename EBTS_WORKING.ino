/*   
 *    ELECTRONIC BLOCK TOKEN SYSTEM  programme   by Michael Beer  13/08/20
 *    
 *    19/08/20    WORKS - software and hardware   :-)
 *    
 *    Analysed and tidied by Magnus Michaelson    09/08/20
 
for the Cader Ifor Railway - a Welsh narrow gauge railway with two stations (terminus and partway) continuing into a cassette representing the rest of the line
Two sections requiring three Token INSTRUMENTS A, B and C plus 2 buttons for D

A MODEL SYSTEM WHERE ANY TRAIN ENTERING A SINGLE LINE SECTION MUST BE IN POSESSION OF A TOKEN AND ONLY ONE TOKEN CAN BE RELEASED FOR A SECTION
TAKE levers allow token to be taken (once LEVERS are unlocked by servos)
  Servos a, B, & C move to unlock TAKE LEVERS for A, B, & C when the block is free and no token is out
     servos also indicate AVAILABLE by microswitch operating an LED
PUT levers unlock slot to allow insertion of token and notify program token has been inserted
  Lever monitored in the program by microswitches
  servos individually calibrated under test   -   approximately unlock @ 20    lock @ 160

BLOCK ENTRY SIGNALS operated by the programme
Signals returned to danger by trains passing IR Detectors ( into Analog ports)
Panel A - Llanfair up trains    Panel B - Bethel down trains   Both control lower block section  -  AB
Panel C - Bethel to the rest of the line -  2 buttons for D

*/
#include <VarSpeedServo.h>
VarSpeedServo myservo1;  // create servo object to control A servo
VarSpeedServo myservo2;  // create servo object to control B servo
VarSpeedServo myservo3;  // create servo object to control C servo

// variables for reading the lever status
bool AtakeLock = false;  //  false = low = operated
bool BtakeLock = false;
bool CtakeLock = false;
bool DtakeLock = false;

int Dtakemon = 0;
int Dputmon = 0;

bool AputLock = false;
bool BputLock = false;
bool CputLock = false;
bool DputLock = false;

bool Asignal = false;   // true = train may enter section
bool Bsignal = false;
bool Csignal = false;

bool blockedAB = false;
bool blockedCD = false;
bool Init = false;


// variables to store the values coming from the  IR detectors
int IRDA = 0;  int IRDB = 0;  int IRDC = 0;
//   IRDA on  A0;  IRDB on A1;  IRDC on A2;



void setup() {

// declare digital pin LEDs as  outputs for signals.
  pinMode(2, OUTPUT);    //  A
  pinMode(3, OUTPUT);    //  B
  pinMode(4, OUTPUT);    //  C

// declare digitals pin as  input for TAKE knobs / buttons.
   pinMode(5, INPUT);    //  A
   pinMode(6, INPUT);    //  B
   pinMode(7, INPUT);    //  C
   pinMode(A4, INPUT);   //  D  - analog

// declare digital pins as  input for PUT knobs / buttons.
   pinMode(8, INPUT);     //  A
   pinMode(12, INPUT);    //  B
   pinMode(13, INPUT);    //  C
   pinMode(A5, INPUT);    //  D  -  analog

// declare analog pins as input for IRDs
   pinMode(A0, INPUT);     //  A
   pinMode(A1, INPUT);     //  B
   pinMode(A2, INPUT);     //  C

   Serial.begin(9600);
}


void loop() {
   if(Init == false) {   //   start with servos unlocked for take
      myservo1.attach(9);
      myservo1.write(10, 15,true);
      myservo1.detach();
      myservo2.attach(10);
      myservo2.write(15, 15, true);
      myservo2.detach();
      myservo3.attach(11);
      myservo3.write(40, 15,true);
      myservo3.detach();
      Init = true;
   }
//...............................................A BLOCK
  if (blockedAB == false) {    
    AtakeLock = digitalRead(5);
    if (AtakeLock == false ){
      while(AtakeLock==false){
        AtakeLock = digitalRead(5);
      }    
      blockedAB = true;     //  AB now blocked
      myservo1.attach(9);
      myservo1.write(160, 15,true);
      myservo1.detach();
      myservo2.attach(10);
      myservo2.write(150, 15,true);
      myservo2.detach();
      digitalWrite(2, HIGH);
      Asignal = true;
    }
  }
  if (blockedAB == true){     //   PUT routine
    AputLock = digitalRead(8);
    if (AputLock == false) {
      while(AputLock==false) {  //  wait for lever release
        AputLock = digitalRead(8);
      }
      blockedAB = false;
      myservo1.attach(9);
      myservo1.write(10, 15,true);
      myservo1.detach();
      myservo2.attach(10);
      myservo2.write(10, 15, true);
      myservo2.detach();
    }
  }
  if(Asignal == true) {    //   on
    IRDA = analogRead(A0);
    if (IRDA < 500 ){     //   train  passing
      digitalWrite(2, LOW);
      Asignal = false;
    }
  } 
  //....................................B BLOCK
  if (blockedAB == false){
    BtakeLock = digitalRead(6);
    if (BtakeLock == false){
      while(BtakeLock == false){
        BtakeLock = digitalRead(6);
      }    
      blockedAB = true;
      myservo2.attach(10);
      myservo2.write(150, 15,true);
      myservo2.detach();
      myservo1.attach(9);
      myservo1.write(160, 15,true);
      myservo1.detach();
      digitalWrite(3, HIGH);
      Bsignal = true;
    }
  }
  if (blockedAB == true){      
    BputLock = digitalRead(12);    //     Bput opened
    if (BputLock == false) {
      while(BputLock==false) {  //  wait for lever release
        BputLock = digitalRead(12);
      } 
      blockedAB = false;    //         A & B tokens available
      myservo1.attach(9);
      myservo1.write(10, 15,true);
      myservo1.detach();
      myservo2.attach(10);
      myservo2.write(10, 15, true);
      myservo2.detach();
    }
  }
  if(Bsignal = true ){
    IRDB = analogRead(A1);
    if (IRDB < 500) {
      digitalWrite(3, LOW);
      Bsignal = false;
    }
  }
//...........................................C BLOCK

  if (blockedCD == false) {      
    CtakeLock = digitalRead(7);
    if (CtakeLock == false ){
      while(CtakeLock==false){
        CtakeLock = digitalRead(7);
      } 
      blockedCD = true;     //  CD now blocked
      myservo3.attach(11);   
      myservo3.write(150, 15,true);
      myservo3.detach();
      digitalWrite(4, HIGH);   //           C signal ON
      Csignal = true;
    }
  }
  if (blockedCD == true){    
    CputLock = digitalRead(13);
    if (CputLock == false) {
      while(CputLock==false) {  //  wait for lever release
        CputLock = digitalRead(13);
      blockedCD = false;
      myservo3.attach(11);
      myservo3.write(30, 15,true);  //  unlock C take
      myservo3.detach();
    }
    }
  }
  if(Csignal == true) {    //   signal on
    IRDC = analogRead(A2);
    if (IRDC < 500 ){     //   train  passing
      digitalWrite(4, LOW);
      Csignal = false;
    }
  }
//     .................................D BLOCK
  
    if (blockedCD == false){
      Dtakemon = analogRead(A5);
        if (Dtakemon < 500) {
           DtakeLock = false;       
        }
        if(DtakeLock==false){
          while(DtakeLock == false){
            Dtakemon = analogRead(A5);        // read Dtake button
            if (Dtakemon > 500) {
                 DtakeLock = true;
            }
          }
         blockedCD = true;
         myservo3.attach(11);
         myservo3.write(150, 15,true);
         myservo3.detach();
     }
    }  
  if (blockedCD == true ){       
    Dputmon = analogRead(A4);   //   Dput button
    if(Dputmon < 500) {
       DputLock = false;  // LOW  //  OPERATED
    }
    if (DputLock == false) {
      while(DputLock==false) {  //  wait for PUT button release
         Dputmon = analogRead(A4);
         if(Dputmon > 500) {   
              DputLock = true;  //  HIGH  //  RELEASED
         }
      }
      blockedCD = false;    //         C & D tokens available
      myservo3.attach(11);   //   C servo
      myservo3.write(30, 15,true);
      myservo3.detach();
   }
 }
} // LOOP BACK
