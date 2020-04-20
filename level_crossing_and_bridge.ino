//  servo operated level crossing gates  stepper raising bridge
//  code   MJB 
//  bridgecheck added 07/04/20   compiles
//  using Arduino with LM293 motor shield and Nema17 stepper
//  enough Digi pins if only 1 flashing light
//  9 & 10 for servos,  leaves    2  for one lights, and 13 for speaker
//  gates based on  Railway Modeller December 2018
//  NOTE servos must rotate in opposite directions    /     swapped parameters for second gate
//   switch for gates (A2) and bridge (A4)  and   (A0) for down microswitch,

#include <VarSpeedServo.h>  
VarSpeedServo myservo1;  
VarSpeedServo myservo2;   

#include <AFMotor.h>    //   stepper + LM293 + 2 servos
//  const int stepsPerRevolution = 200;    // for Nema 17
// connect stepper motor to port #2 (M3 and M4)
AF_Stepper motor(200, 2);      
      
int gatepos=0;     // 2 for traffic  -  1  for trains
int beep=0;
int gatebutton=1;

int bridgepos=0;   // 0 =  down   UP = 1
int switchread = 1023;
int bridgebutton=1;
int stepTravel=200;   //  constant - depends on gear ratios 
int bridgecheck=1;    //  do check first run
int closed=1;

void setup(){
//  pinMode(3, INPUT);   //   pin3   freed from operate gates switch
//  pinMode(1,INPUT);   //    pin 1  freed from bridge button
  pinMode(2, OUTPUT);    //  flashing led 
  pinMode(13,OUTPUT);  //       speaker     pin 13 to GND  8ohm speaker
  pinMode(A0,INPUT);  // //  A0   bridge down microswitch
  pinMode(A2,INPUT);  //     gates button
  pinMode(A4,INPUT);  // //   bridge button


}
void loop (){

//  Initialise bridge Down
//   initialise door position routine 
   while (bridgecheck >0) { // hasn't been initialised
      switchread = (analogRead(A0));   //  1023 if open
      if (switchread>200) {
        closed = 0;     //  not closed
      }
      while ( closed == 0) {    
        motor.step(10, FORWARD, MICROSTEP);    //  door close a bit  
        switchread = (analogRead(A0));
        if(switchread <10) { //   microswitch closed 
          closed = 1;
          motor.release();
          bridgecheck = 0;
       }
    } 
   }  

//                             initialise gate servos first time around
//                             positions from passenger crossing   -   ALTER
  if(gatepos == 0){
     myservo1.attach(9);             //  down gate
     myservo1.write (135, 5, true);  // to across track
     myservo1.detach();
     myservo2.attach(10);            //  up gate
     myservo2.write (45, 5, true);   //   to across track
     myservo2.detach();
     gatepos=2;                      //   gates favour road
  }

//   gate control
     gatebutton = analogRead(A2);
    if(gatebutton == LOW) {          //  for trains   //  road closing
       if(!(gatepos == 1)){       

//                                   neenaw
          beep = 0;
        do {
           digitalWrite(2, HIGH); //  red on    
           tone(13, 1100, 300);   //  high beep
           delay(300);                       
           digitalWrite(2, LOW);  //  red off;  
           tone(13, 900, 300);    //  low beep
           delay(300);
           beep = beep + 1;
    } 
       while( beep <20);
         digitalWrite(2, HIGH);           //   red stays on 
                    
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
         gatepos=1;        
                                        // gates closed   -  ok to raise bridge
    
//      RAISE / LOWER BRIDGE ROUTINE                    only IF gates closed 
//      NOT READING  LIMIT SWITCH   (A0)
//      could do ?    TRUST Stepper drive

          if(!(bridgepos == 1)){                         //  if it's down 
              bridgebutton = analogRead(A4);                  
                 if(bridgebutton >10) {                  //  then  lift  bridge 
                   beep = 0;
                   while( beep <10){                     //   ten beeps before moving
                     tone(13, 1100, 300); 
                     delay(300);                         
      //               tone(13, 900, 300); 
      //               delay(300);
                     beep = beep + 1;
                  } 
              }
                 motor.step(stepTravel, FORWARD, SINGLE);  //  assume forward is raise  
                 bridgepos = 1;                            //   it's up now
                 
        }else{
          
          if(!(bridgepos == 0)){                          // if it's up
               bridgebutton = analogRead(A4);
               if(bridgebutton > 10) {                    //  then   lower bridge        
                   beep = 0;
                   while( beep <10){                      //   ten beeps before moving                        
                     tone(13, 900, 300); 
                     delay(300);
                     beep = beep + 1;
                  } 
                  motor.step(stepTravel, BACKWARD, SINGLE); 
                  bridgepos = 0;                         //  it's down now 
         }
      }
    }
   //   END OF BRIDGE ROUTINE
    
  }else{
 
     if(!(gatepos == 2)){   
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
        gatepos=2;
    }
   }
  }
}    //  loop back

  
