//  Random nested rabbit loops  25 August
/*  two loops nested   one  prep for a random count outer for multiplier
   WORKS   28/08/25

  pin 6   Servo1     rabbit
  Pin 7   Servo2     gnome
  pin A0 open to give noise to seed gen
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <VarSpeedServo.h>
VarSpeedServo myservo1;  //  pin6     Rabbit hole
VarSpeedServo myservo2;  //  pin7     gnome

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

void setup() {
  Serial.begin(9600);
  pinMode(6, OUTPUT);   //  Rabbit servo
  myservo1.attach(6);
  pinMode(7, OUTPUT);   //  gnome servo
  myservo2.attach(7);
}

void loop() {
   outerloopR();
   outerloopG();
}   //   end of main loop

void Random() {   //  generate a random number 0 to 9999
  randomSeed(analogRead(0));
  // get a random number from 100 to 9999
  randNumber = random(10, 9999);
  copyOfRand = randNumber;
  delay(50);
}   //  end of Random


void innerloopR() {
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
  else{
    Random();
    RndRabbit = randNumber;   
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
    myservo1.write(10, 100, true);
    upDown = LOW;
  }
  else {
    if (upDown == LOW) {  //if DOWN go UP
      myservo1.write(170, 10, true);
      upDown = HIGH;
    }
  }    //   ELSE
}   //   endproc

void innerloopG() {
   
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
  else{
    Random();
    RndGnome = copyOfRand;   
    Gouter = 0;
    Ginner = 0;
    GnomeCast();
  }
}

void GnomeCast() {   //  (gnome action)
  //  sequence of cast and back
  Serial.print("Gnomecast  ");
  Serial.println(RndGnome);

  Gcasts = 0;
  while (Gcasts < 3) {
  Gcasttime = millis; 
    myservo2.write(25, 100, true);
    if (Gcasttime + 2000){
 //       Serial.print("  millis ");
 //       Serial.print(Gcasttime + 20000);  
    myservo2.write(110, 10, true);
    }
    //if (Gcasttime + 20400){    
    Gcasts = Gcasts + 1;
  }
}   //   endproc
