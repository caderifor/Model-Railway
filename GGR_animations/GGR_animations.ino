/* Random nested rabbit loops + weatherhog   19.09/25
  From   two loops nested   one  prep for a random count outer for multiplier
   RABBIT AND GNOME WORKED   28/08/25
   Wind vane also works on serial monitor

   
  pin 4   Blink     if used
  pin 6   Servo1     rabbit
  Pin 7   Servo2     gnome
  pim 9   Servo3     Wind direction

  pin A0 open to give noise to seed gen
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <VarSpeedServo.h>
VarSpeedServo myservo1;  //  pin6     Rabbit hole
VarSpeedServo myservo2;  //  pin7     gnome
VarSpeedServo myservo3;  //  pin9     Wind

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
  pinMode(6, OUTPUT);   //  Rabbit servo
  myservo1.attach(6);
  pinMode(7, OUTPUT);   //  gnome servo
  myservo2.attach(7);
  pinMode(9, OUTPUT);   //  wind servo
  myservo3.attach(8);
}

void loop() {
  outerloopR();
  outerloopG();
  outerloopW();
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
  //  reel in slow  cast fast - 3 times

  Gcasts = 0;
  while (Gcasts < 3) {
    Gcasttime = millis;
    myservo2.write(10, 10, true);

    if ((Gcasttime + 2000) > millis) {

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
