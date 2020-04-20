//  station crossing servos and stepper doors
//   incorporating stepper motor CD drive  with initialise
//   working 06/04/20
//   PIR sensor  pin 2   Limit switch pin A0    Gate change pin 13
//   gate angle values increase with anticlockwise rotation seen from above
// 
 #include <VarSpeedServo.h>  
VarSpeedServo myservo1;  //  pin9
VarSpeedServo myservo2;  //  pin 10
int gatepos=2;     // 2 for passengers  -  1  for locos

#include <AFMotor.h>
AF_Stepper motor(200, 2);  //  motor shield port 2
int doorcheck=2;     //  > 0  means not init yet

//  PIR sensor
int inputPin = 2;  // PIR sensor pin 2
int openTime = 2000;
int pinval = 0;      //  analog reading for PIR detecting motion
int closed = 0;   //  door microswitch pulls A0 low and gives 1 for closed
int switchread;   // microswitch read - analog range changed to 0 or 1

void setup(){
  //  set up serial library at 9600 bps
  Serial.begin(9600);
  motor.setSpeed(16);   //  door speed
pinMode(13, INPUT);    //  pin 13 for change gates button
pinMode(A0, INPUT);    //  ANALOG FOR DOOR CLOSED LIMIT SWITCH
pinMode(2, INPUT);     //  PIR
}
  
void loop() {    
  
//   initialise door position routine 
   while (doorcheck >0) { // hasn't been initialised
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
          doorcheck = 0;
       }
    } 
   }     
   
    if (closed == 1){
      pinval = digitalRead(2);  //  read PIR input
      if (pinval == HIGH) {   //  check for input high
        motor.step(200, BACKWARD, MICROSTEP);  //   door open code 
        motor.release(); 
        closed = 0;   // we just opened the door
        openTime = 0;
      }
  } 
  else 
  {
    if (closed == 0) {   //  door open
      if (openTime > 2000){   //   door has been open 2000 prog cycles
          motor.step(200, FORWARD, MICROSTEP);    //  door close  
          motor.release();       
        closed = 1; 
     }       
  }
 }  
   if(closed==0){   //   if door is open  count up one
     openTime=openTime + 1;   //  every program cycle
  }

//      crossing control
   if(digitalRead(13) == LOW){         //  gate change button
     if(!(gatepos == 2)) {            //  passengers call but not set
      myservo1.attach(9);                //     DOWN END GATE servo on pin 9  
      myservo2.attach(10);               //     UP END GATE servo from  pin 10       -  
//    to passengers positions
      myservo1.write(155, 5, true);     //  DOWN gate to setback 
      myservo2.write(135, 5, true);     //  UP gate  across track
      myservo1.write(135, 5, true);     //  DOWN gate  across track 
      myservo1.detach();            
      myservo2.detach(); 
      gatepos=2;        //       servos now at passennger position
 }
}
 //   read switch for change
    if(digitalRead(13) == LOW){         //  change button
      if(!(gatepos == 1)) {             //  locos call but not set    
      myservo1.attach(9);                //     DOWN END GATE servo on pin 9  
      myservo2.attach(10);               //     UP END GATE servo from  pin 10       -  
//    to locos positions
      myservo1.write(155, 5, true);    //  DOWN gate to setback
      myservo2.write(20, 5, true);   //  UP gate to block passengers
      myservo1.write(45, 5, true);   //  DOWN gate in line with track
      myservo1.detach();            
      myservo2.detach(); 
      gatepos=1;               //  servos now at loco position  
  }
 } 
}   // back to void loop
