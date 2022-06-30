//   Pan and Tilt camera mount using 2 servos- Procedurised -
//   Michael Beer  
//     11/06/22    COMPILES     :-)
//     14/06/22    Debug version
//     15/06/22    BEST USE OF Servos ?? 
//                 compiles
//     16/06/22    CHECKed max angles all thru  -  180 
//                 seems to work as far as hardware allows

//   180 degree travel -  mechanically gear down  or use only centre of servo range ?
//   best to limit angles for mechanical reasons
//   decide later


//   Potentiometers on A0 and A2
//   Preset buttons (external pullups) on 3, 4, 5 and store button on 6 and Follow on 12
//   Goto button 13 has internal pullup
//   servos on 9 & 10
//   LEDs on 7, 8  &11    (330R lim)


#include <VarSpeedServo.h>
VarSpeedServo myservo1;  //  pin9     Az
VarSpeedServo myservo2;  //  pin 10   El

//    servos 0 - 180
//    Az and El  angles handled as degrees 0 to 180


int Xknob = 0;
int Yknob = 0;
int XlastSet = 90;
int YlastSet = 90;
bool Pos1    = HIGH;     //  button (not pressed)
bool Pos2    = HIGH;     //  button
bool Pos3    = HIGH;     //  button
bool Goto    = HIGH;     //  button
bool Store   = HIGH;     //  button
bool Follow  = HIGH;     //  button
int Xpos1   = 90;    //  Start positions   -  mid way
int Ypos1   = 90;
int Xpos2   = 80;
int Ypos2   = 80;
int Xpos3   = 100;
int Ypos3   = 100;
int AZservo = 90;
int ELservo = 90;
int Az      = 90;
int El      = 90;
bool Init   = LOW;    //  initiated ?  check


void setup() {
  Serial.begin(9600);

  pinMode(A0, INPUT);    //  ANALOG El pot
  pinMode(A2, INPUT);    //  ANALOG Az pot
  pinMode(3, INPUT);     //  Preset 1  Button
  pinMode(4, INPUT);     //  Preset 2
  pinMode(5, INPUT);     //  Preset 3
  pinMode(6, INPUT);     //  Store
  pinMode(12, INPUT);    //  FOLLOW button
  pinMode(13, INPUT);    //  GOTO button     //   REM  internal pullup
  pinMode(7, OUTPUT);    //  Preset 1 LED
  pinMode(8, OUTPUT);    //  Preset 2 LED
  pinMode(11, OUTPUT);   //  Preset 3 LED

}

void loop() {

  Serial.println("loop");
  
  Serial.print("Az ");
  Serial.print(Az);
  Serial.print("El ");
  Serial.print(El);  
  Serial.print(" XlastSet ");
  Serial.print(XlastSet);
  Serial.print(" YlastSet ");
  Serial.print(YlastSet);
  Serial.print(" Xpos1 ");
  Serial.print(Xpos1);
  Serial.print(" Ypos1 ");
  Serial.print(Ypos1);
  Serial.print(" Xpos2 ");
  Serial.print(Xpos2);
  Serial.print(" Ypos2 ");
  Serial.print(Ypos2);
  Serial.print(" Xpos3 ");
  Serial.print(Xpos3);
  Serial.print(" Ypos3 ");
  Serial.println(Ypos3);

  //  Init routine    //   once only
  if (Init == LOW) { // hasn't been initialised  //  else skip
    Serial.println("Init servos to mid position");
    myservo1.attach(9);                //    Az servo pin 9    centre
    myservo1.write(90, 5, true);
    myservo1.detach();

    myservo2.attach(10);               //     El servo from  pin 10       level-
    myservo2.write(90, 5, true);
    myservo2.detach();
    digitalWrite(7, HIGH);   //   LED 1
    Init = HIGH;
    Serial.println("Az and El initialised midway on Preset 1");
  }

  //    NORMAL LOOP starts here
  //      diagnostics buttons
  Follow = digitalRead(12);
  Goto =  digitalRead(13);
  Store = digitalRead(6);
  Pos1  = digitalRead(3);
  Pos2  = digitalRead(4);
  Pos3  = digitalRead(5);

  if (Goto == LOW) {
    Serial.print("Goto ");
  }
  if (Store == LOW) {
    Serial.print(" Store ");
  }
  if (Pos1 == LOW) {
    Serial.print(" Pos1 ");
  }
  if (Pos2 == LOW) {
    Serial.print(" Pos2 ");
  }
  if (Pos3 == LOW) {
    Serial.println(" Pos3 ");
  }

 
  //    Determine mode


  //  FOLLOW MODE ?
  Follow = digitalRead(12);
  if (Follow == LOW) {
    followProc();
  } else  {

    //     GOTO to selected position buttons
    Goto  = digitalRead(13);
    Pos1  = digitalRead(3);
    Pos2  = digitalRead(4);
    Pos3  = digitalRead(5);

    if (Goto == LOW) {   //  Goto pressed   

      if (Pos1 == LOW) {
        drivePos1();
      }
      if (Pos2 == LOW) {
        drivePos2();
      }
      if (Pos3 == LOW) {
        drivePos3();
      }
    }
  }  //   end GOTO


  //  read Store current position mode buttons

  Store = digitalRead(6);
  Pos1  = digitalRead(3);
  Pos2  = digitalRead(4);
  Pos3  = digitalRead(5);
  if (Store == LOW) {   //  Store pressed   conflict with save position **

    if (Pos1 == LOW) {
      Xpos1 = XlastSet;
      Ypos1 = YlastSet;
      digitalWrite(7, HIGH);   //   LED 1
      
      delay(100);
      digitalWrite(7, LOW);  
    }

    if (Pos2 == LOW) {
      Xpos2 = XlastSet;
      Ypos2 = YlastSet;
      digitalWrite(8, HIGH);   //   LED 2
      delay(100);
      digitalWrite(8, LOW);     

    }
    if (Pos3 == LOW) {
      Xpos3 = XlastSet;
      Ypos3 = YlastSet;
      digitalWrite(11, HIGH);   //   LED 3
      delay(100);
      digitalWrite(11, LOW);  
    }
  }   //   end STORE

}      // back to void loop


void drivePos1() {
  //  turn other LEDs off
  digitalWrite(8, LOW);
  digitalWrite(11, LOW);
  Az = Xpos1;      //
  El = Ypos1;
  myservo1.attach(9);                //    Az servo pin 9
  myservo1.write(Az, 5, true);
  myservo1.detach();

  myservo2.attach(10);               //     El servo from  pin 10       -
  myservo2.write(El, 5, true);
  myservo2.detach();
  digitalWrite(7, HIGH);             //  Pos 1 achieved
  Serial.println("Position 1 achieved");
}

void drivePos2() {
  //  turn other LEDs off
  digitalWrite(7, LOW);
  digitalWrite(11, LOW);
  Az = Xpos2;
  El = Ypos2;
  myservo1.attach(9);               //  Az servo on pin 9
  myservo1.write(Az, 5, true);      //  move Az servo
  myservo1.detach();
  myservo2.attach(10);              //    El servo on  pin 10       -
  myservo2.write(El, 5, true);      //  Move El servo
  myservo2.detach();
  digitalWrite(8, HIGH);             //  Pos 2 achieved
  Serial.println("Position 2 selected");
}

void drivePos3() {
  //  turn other LEDs off
  digitalWrite(8, LOW);
  digitalWrite(7, LOW) ;
  Az = Xpos3;
  El = Ypos3;
  myservo1.attach(9);                //    Az servo pin 9
  myservo1.write(Az, 5, true);
  myservo1.detach();
  myservo2.attach(10);               //     El servo from  pin 10       -
  myservo2.write(El, 5, true);
  myservo2.detach();
  digitalWrite(11, HIGH);             //  Pos 3 achieved
  Serial.println("Position 3 selected");
}



//    THIS IS WHERE THE STORY GETS INTERESTING

void followProc() {

  //   called when  Follow button pressed
    Serial.print("           Following ");

    
  myservo1.attach(9);               //  Az servo on pin 9
  myservo2.attach(10);              //    El servo on  pin 10
    digitalWrite(7,LOW);
    digitalWrite(8,LOW);
    digitalWrite(11,LOW); 
  
  while (Follow == LOW) {

    Xknob = (analogRead(A0));    //   0 t0 1024
    Xknob = Xknob / 6;           //  0 to 180 to SERVO   Divide by 5.68888888   say 6
    Az = Xknob;                  //  For position memory - XlastSet
    Yknob = (analogRead(A2));
    Yknob = Yknob / 6;
    El = Yknob;
    myservo1.write(Az, 5, true);      //  move Az servo
    myservo2.write(El, 5, true);      //  Move El servo

    digitalWrite(7,HIGH);
    digitalWrite(8,HIGH);
    digitalWrite(11,HIGH);
    delay(10);
    digitalWrite(7,LOW);
    digitalWrite(8,LOW);
    digitalWrite(11,LOW);  
    
    Follow = digitalRead(12);      //  is  button released (HIGH) ?
     }   //   end condition for while

         
  XlastSet = Az;
  YlastSet = El;
  myservo1.detach();
  myservo2.detach();
}   

//  endproc followProc

// MMI works thus....  Op holds FOLLOW and moves pot to guessed position
//  camera moves to guessed position and operator refines guess
//  camera moves etc  until FOLLOW is released
//  incorporate a flash on each acquisition as
//  prompt to release Follow if satisfied





//  END OF CODE
