//   Pan and Tilt camera mount using 2 servos- Procedurised -
//   Michael Beer   started 27/11/21


//                  Chatter problem - dirty supply of 10K pot too high ?
//    20/06/22     cut down to test for AVERAGING
//    27/06/22     PROVES PROBLEM IS BAD SPOTS ON POTS

//   Potentiometers on A0 and A2
//   servos on 9 & 10



#include <VarSpeedServo.h>
VarSpeedServo myservo1;  //  pin9     Az
VarSpeedServo myservo2;  //  pin 10   El


int Xknob = 0;
int Yknob = 0;
int XlastSet = 90;
int YlastSet = 90;

int Xpos1   = 90;    //  Start positions   -  mid way
int Ypos1   = 90;
int Xpos2   = 80;
int Ypos2   = 80;
int Xpos3   = 100;
int Ypos3   = 100;
int AZservo = 90;
int ELservo = 90;
int Azav    = 0;
int AzlastSet = 0;
int count   = 0;
int Elav    = 0;
int EllastSet = 0;
int Az      = 90;
int El      = 90;
int Ecount  = 0;
int Azer    = 0;
int Azprev  = 0;
int Eler    = 0;
int Elprev  = 0;


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

  Az = (analogRead(A0));
  Azer = abs(Azprev - Az);
  if (Azer < 4) {
    Az = Azprev;
  }
  Serial.print(Az);
  Serial.print("  -  ");
  Azprev = Az;

  El = (analogRead(A2));
  Eler = abs(El - Elprev);
  if (Eler < 4) {
    El = Elprev;
  }
  Serial.println(El);
  Elprev = El;


}                //  end loop
