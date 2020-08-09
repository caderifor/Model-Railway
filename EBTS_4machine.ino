/*
 * 
 * Doubt over D buttons reading    07/08/20
 * 
ELECTRONIC BLOCK TOKEN SYSTEM  programme ex EBTS_20April   20/04/20  WORKING   :-)
  by Michael Beer
Modification to include virtual machine D - just Put and Take buttons - on Analog 4 & 5
12/06/20
Machines fitted with AVAILABLE LED microswitches and servo positions tweaked

 
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
        
Rejigged to wait for release of TAKE knobs - So servos do not foul on locking cam.   17/04/20 
 D

Code for A & B used for C & D with the names changed and servo and signal code removed from D (virtual machine)

           
*/
#include <VarSpeedServo.h>
VarSpeedServo myservo1;  // create servo object to control A servo
VarSpeedServo myservo2;  // create servo object to control B servo
VarSpeedServo myservo3;  // create servo object to control C servo

// variables for reading the lever status
bool AtakeLock = false;         
bool BtakeLock = false;
bool CtakeLock = false;
bool DtakeLock = false;
int Dtakemon = 0;
int Dputmon = 0;
//
bool AputLock = false;
bool BputLock = false;
bool CputLock = false;
bool DputLock = false;

bool Asignal = false;   // train has entered section
bool Bsignal = false;
bool Csignal = false;

// blocked variables
bool blockedAB = false;
bool blockedCD = false;

bool aHold = false;      //   indicates if other end holds a token
bool bHold = false;
bool cHold = false;
bool dHold = false;

int timewaste = 1000;    //  diagnostic only

// variables to store the values coming from the  IR detectors
int IRDA = 0;  int IRDB = 0;  int IRDC = 0; 
//   IRDA on  A0;  IRDB on A1;  IRDC on A2; 



void setup() {

// declare digital pin LEDs as  outputs for signals.    
  pinMode(2, OUTPUT);    //  A
  pinMode(3, OUTPUT);    //  B
  pinMode(4, OUTPUT);    //  C
 
// declare digitals pin as  input for TAKE buttons.
   pinMode(5, INPUT);    //  A
   pinMode(6, INPUT);    //  B
   pinMode(7, INPUT);    //  C
   pinMode(A4, INPUT);   //  D  - analog

// declare digital pins as  input for PUT buttons.
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


       Serial.println("A block");
       Serial.println(blockedAB); 
       Serial.println(bHold); 
       if (blockedAB == true && bHold == false){     //   PUT routine - only do if b not holding token
          AputLock = digitalRead(8);
             Serial.println(AputLock);   
          if (AputLock == false) {
              Serial.println("A put opened - insert token");
              while(AputLock==false) {  //  wait for lever release
                AputLock = digitalRead(8); 
              } //   end of wait             
                Serial.println("A token inserted - block free");
                blockedAB = false; 
                aHold = false;
  //  Unlock A and B take levers
               myservo1.attach(9); 
               myservo1.write(10, 15,true); 
               myservo1.detach();   
               Serial.println("A servo unlocked");
               myservo2.attach(10); 
               myservo2.write(10, 15, true);
               myservo2.detach();  
               Serial.println("B servo unlocked"); 
             }
          }  
     
    if (blockedAB == false) {      //   if AB not blocked  (token available)
           AtakeLock = digitalRead(5);  
           if (AtakeLock == false ){ 
                  Serial.println(" A take lever operated");             
                  while(AtakeLock==false){                                                                
                       AtakeLock = digitalRead(5); 
                  }     //  end of while  = true
                       Serial.println("A take lever released"); 
 
                  blockedAB = true;     //  AB now blocked
                  aHold = true;  
                       myservo1.attach(9);
                       myservo1.write(170, 15,true); 
                       myservo1.detach();
                       Serial.println("A servo locked");
                       myservo2.attach(10);
                       myservo2.write(150, 15,true);
                       myservo2.detach();
                       Serial.println("B servo locked"); 
                       digitalWrite(2, HIGH);
                       Asignal = true;       
                       Serial.println("A signal on");
          } 
    }           
            if(Asignal == true) {    //   on
                IRDA = analogRead(A0);
                if (IRDA < 500 ){     //   train  passing
                   digitalWrite(2, LOW);
                   Serial.println("A signal off"); 
                   Asignal = false; 
                }
           }
                    delay(timewaste);         
  
       Serial.println("B Block");
              Serial.println(blockedAB); 
              Serial.println(aHold); 
       if (blockedAB ==  true && aHold == false){      //  only if  A hasn't got the token

          BputLock = digitalRead(12);    //     Bput opened
         if (BputLock == false) { 
             Serial.println("B put pressed - insert token");          
             while(BputLock==false) {  //  wait for lever release
                BputLock = digitalRead(12);   
             }  //  end of while  = true             
                   Serial.println("B put lever released");
                   blockedAB = false;    //         A & B tokens available 
                   bHold = false;
    //             A & B Blocks free, unlock
               myservo1.attach(9); 
               myservo1.write(10, 15,true); 
               myservo1.detach();   
               Serial.println("A servo unlocked");
               myservo2.attach(10); 
               myservo2.write(10, 15, true);
               myservo2.detach();  
               Serial.println("B servo unlocked"); 
                }
           }   
        
         if (blockedAB == false){ 
                BtakeLock = digitalRead(6);       // read Btake button             
                if (BtakeLock == false){
                         Serial.println("B take lever operated");
                   while(BtakeLock == false){
                      BtakeLock = digitalRead(6);                  
                }    //    end of while  = true
                   Serial.println("B take lever released");
 
                blockedAB = true;
                bHold = true;
                myservo2.attach(10);
                myservo2.write(150, 15,true);
                myservo2.detach();
                Serial.println("B servo locked");
                myservo1.attach(9);
                myservo1.write(170, 15,true); 
                myservo1.detach();
                Serial.println("A servo locked"); 
                digitalWrite(3, HIGH);
                Serial.println("B signal on"); 
                Bsignal = true;
              }
         }
         
     if(Bsignal = true ){
           IRDB = analogRead(A1);
           if (IRDB < 500) {
             digitalWrite(3, LOW); 
             Serial.println("B signal off"); 
             Bsignal = false;
         }
     }
                       delay(timewaste);


       Serial.println("C block");
              Serial.println(blockedCD); 
              Serial.println(dHold); 
       if (blockedCD == true && dHold == false){     //   only do if b not holding token
          CputLock = digitalRead(13);    
          if (CputLock == false) {
              Serial.println("C put pressed - insert token");
              while(CputLock==false) {  //  wait for lever release
                CputLock = digitalRead(13);    
              } //   end of wait             
                Serial.println("C token inserted - block free");
                blockedCD = false; 
                cHold = false;
  //  Unlock C  take lever
               myservo3.attach(11);     
               myservo3.write(30, 15,true); 
               myservo3.detach();   
               Serial.println("C servo unlocked");
             }
          }  
      
    if (blockedCD == false) {      //   if CD not blocked  (token available)
           CtakeLock = digitalRead(7);    
           if (CtakeLock == false ){ 
                  Serial.println(" C take lever operated");             
                  while(CtakeLock==false){                                                                
                       CtakeLock = digitalRead(7);   
                  }     //  end of while  = true
                       Serial.println("C take lever released"); 
                       
                  blockedCD = true;     //  CD now blocked
                  cHold = true;  
                       myservo3.attach(11);    //   (??)  
                       myservo3.write(150, 15,true); 
                       myservo3.detach();
                       Serial.println("C servo locked");
                       digitalWrite(4, HIGH);     //   (??)  
                       Csignal = true;       
                       Serial.println("A signal on");
          } 
    }           
            if(Csignal == true) {    //   signal on
                IRDC = analogRead(A0);     
                if (IRDC < 500 ){     //   train  passing
                   digitalWrite(4, LOW);    
                   Serial.println("C signal off"); 
                   Csignal = false; 
                }
           }
                    delay(timewaste);         

       Serial.println("D Block");
              Serial.println(blockedCD); 
              Serial.println(cHold); 
       if (blockedCD == true && cHold == false){      //  only if  C hasn't got the token

          Dputmon = analogRead(A4);   //   Dput button
          if(Dputmon < 500) {
             DputLock = false;
          }
         if (DputLock == false) { 
             Serial.println("D put pressed - token held "); 

                      
             while(DputLock==false) {  //  wait for PUT button release
                Dputmon = analogRead(A4);  
                if(Dputmon < 500) {
                   DputLock = true; 
                }               
           }  //  end of while false   = true = button released            
                   Serial.println("D put button released");
                   blockedCD = false;    //         C & D tokens available 
                   dHold = false;
    //             C & D Blocks free, unlock C
               myservo3.attach(11);   //   C servo  
               myservo3.write(30, 15,true); 
               myservo3.detach();   
               Serial.println("C servo unlocked");
                }
           }   
        
         if (blockedCD == false){ 
             Dtakemon = analogRead(A5);        // read Dtake button             
             if (Dtakemon < 500) {
                DtakeLock = false;
                Serial.println("D take lever operated");              
             }

             while(DtakeLock == false){
                Dtakemon = analogRead(A5);        // read Dtake button             
                if (Dtakemon > 500) {
                    DtakeLock = true; 
                 }                
             }    //    end of while false  = true
                   Serial.println("D take lever released");
 
                blockedCD = true;
                dHold = true;
            //      no servo for D

                myservo3.attach(11);       
                myservo3.write(150, 15,true); 
                myservo3.detach();
                Serial.println("C servo locked"); 
        }
        Serial.println("loop"); 
 }         // LOOP BACK
