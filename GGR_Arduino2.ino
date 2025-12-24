/* GGR animations + Points      COMPILES   19/12/25
   Servos always energised   RECOMMRNDED
   Inputs and outputs rewired to work with Arduino 1
   animations still running times (on monitor)
   Analog values set OK     above or below 500

  adding  Servo for points1, 2 & 3
  pin A1   Point 1 call
  pin A2   Point 2 call
  pin A4   point 3 call
  pin 6   Servo1     rabbit
  Pin 3   Servo2     gnome
  pin 9   Servo3     Wind direction
  pin 5   Servo4     point 3
  pin 10  servo5     point 1
  pin 11  servo6     point 2
  pin A0 open to give noise to seed Random gen
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <VarSpeedServo.h>
VarSpeedServo myservo1;  //  pin6     Rabbit hole
VarSpeedServo myservo2;  //  pin7     gnome
VarSpeedServo myservo3;  //  pin9     Wind
VarSpeedServo myservo4;  //  pin5     point 3 servo 
VarSpeedServo myservo5;  //  pin10    point 1 servo
VarSpeedServo myservo6;  //  pin11    point 2 servo
bool point1 = LOW;
int point1call = 0;
bool point2 = LOW;
int point2call = 0;
bool point3 = LOW;
int point3call = 0;

//  random
long copyOfRand = 0;
long randNumber;

//  rabbit
long Rinner = 0;
long Router = 0;
bool upDown = LOW;
bool runRabbit = LOW;
long RndRabbit = 20;

//  gnome
long Ginner = 0;
long Gouter = 0;
bool Gcast = LOW;
long Gcasttime = 0;
long RndGnome = 20;
int  Gcasts = 0;

//  Wind
long Winner = 0;
long Wouter = 0;
long RndWind = 20;
long WindVane = 0;

void setup() {
  Serial.begin(9600); 
  myservo1.attach(6);//    Rabbit servo  
  myservo2.attach(3);//    gnome servo
  myservo3.attach(9);//    wind servo 
  myservo4.attach(5);//    point 3
  myservo5.attach(10);//   point 1
  myservo6.attach(11);//   point 2
}

void loop() {
  outerloopR();
  outerloopG();
  outerloopW();
  Point1();
  Point2();
  Point3();
}   //   end of main loop

void Random() {   //  generate a random number 0 to 9999
  randomSeed(analogRead(0));
  // get a random number from 100 to 9999
  randNumber = random(10, 9999);
  copyOfRand = randNumber;
  delay(50);
}   //  end of Random


void innerloopR() {         //  Rabbit
  if (Rinner < RndRabbit) {
    Rinner = Rinner + 1;
  }
  if (Rinner > (RndRabbit - 1)) {
    Rinner = 0;
    Router = Router + 1;
  }
}

void outerloopR() {
  if (Router < 10000) {
    innerloopR();
  }
  else {
    Random();
    //    RndRabbit = randNumber;
    RndRabbit = copyOfRand;
    Router = 0;
    Rinner = 0;
    rabbitHole();
  }
}

void rabbitHole() {   //  rabbit action
  Serial.print("  RabbitHole   ");
  Serial.println(RndRabbit);
  if (upDown == HIGH) {  //if UP go down
    myservo1.write(15, 100, true);
    upDown = LOW;
  }
  else {
    if (upDown == LOW) {  //if DOWN go UP
      myservo1.write(150, 10, true);
      upDown = HIGH;
    }
  }    //   ELSE
}   //   endproc

void innerloopG() {      //  Gnome
  if (Ginner < RndGnome) {
    Ginner = Ginner + 1;
  }
  if (Ginner > (RndGnome - 1)) {
    Ginner = 0;
    Gouter = Gouter + 1;
  }
}

void outerloopG() {
  if (Gouter < 13000) {
    innerloopG();
  }
  else {
    Random();
    RndGnome = copyOfRand;
    Gouter = 0;
    Ginner = 0;
    GnomeCast();
  }
}

void GnomeCast() {   //  (gnome action)
  Serial.print("  Gnomecast   ");
  Serial.println(RndGnome);
  //  sequence of cast and back
  //  reel in slow - cast fast - 3 times

  Gcasts = 0;
  while (Gcasts < 3) {
    Gcasttime = millis();
    myservo2.write(10, 10, true);
    if ((Gcasttime + 2000) > millis()) {
      myservo2.write(160, 100, true);
    }
    Gcasts = Gcasts + 1;
  }
}   //   endproc

void innerloopW() {        //  wind direction change
  if (Winner < RndWind) {
    Winner = Winner + 1;
  }
  if (Winner > (RndWind - 1)) {
    Winner = 0;
    Wouter = Wouter + 1;
  }
}

void outerloopW() {
  if (Wouter < 10000) {
    innerloopW();
  }
  else {
    Random();
    RndWind = copyOfRand;  //  10 to 9999
    Wouter = 0;
    Winner = 0;
    windDirection();
  }
}

void windDirection() {   //  at random time turn to random wind direction
  Serial.print("  Wind vane   ");
  Serial.print(RndWind);
  Random();              //  for Wind - not delay
  WindVane = copyOfRand;
  WindVane = WindVane / 56;  //   0 to 180
  Serial.print("  wind direction  ");
  Serial.println(WindVane);
  myservo3.write(WindVane, 100, true);

}   //   endproc

void Point1() {   //    PinA1  - input from Ard1
  point1call = analogRead(A1);
//  Serial.print("A1=");
//  Serial.print(point1call);
  if ((point1call < 500 ) && (point1 == HIGH)) {
    myservo5.write(10);
    point1 = LOW;
  }
  else {
    if ((point1call > 500) && (point1 == LOW)) {
      myservo5.write(160);
      point1 = HIGH;
    }
  }
}   //   end point1

void Point2() {   //    PinA2   - input from Ard1
  point2call = analogRead(A2);
//  Serial.print("  A2=");
//  Serial.print(point2call);
  if ((point2call < 500) && (point2 == HIGH)) {
    myservo6.write(10);
    point2 = LOW;
  }
  else {
    if ((point2call > 500) && (point2 == LOW)) {
      myservo6.write(160);
      point2 = HIGH;
    }
  }
}   //   end point2

void Point3() {   //    PinA4   - input from Ard1
  point3call = analogRead(A4);
//  Serial.print("  A4=");
//  Serial.println(point3call);
  if ((point3call < 500 ) && (point3 == HIGH)) {
    myservo4.write(10);
    point3 = LOW;
  }
  else {
    if ((point3call > 500) && (point3 == LOW)) {
      myservo4.write(160);
      point3 = HIGH;
    }
  }
}   //   end point3
