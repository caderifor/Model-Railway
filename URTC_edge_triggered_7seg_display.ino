//  triggered 4x7seg time display for URTC

 //  4 x 7seg LED  readout of time from URTC nominally 1 sec pulses
//  31/07/20     edge trigger pin 2 by edgeDet
// 
//  from 7 LED segments demo counter program from electronicsblog.net.
//   https://www.electronicsblog.net/4-digits-7-segments-led-display-multiplexing-with-arduino/


//    pin 2 clock pulse in
//    from seconds counter  using Millis     procedure addSec()  to be called by falling edge
//    called by edgeDet reading pin 2 


// segment | Arduino UNO board PIN number
#define G 9
#define f 8
#define A 3
#define B 4
#define E 7
#define D 6
#define C 5
//   F switched to f due to a clash with something in library (?)

// Common cathodes control
#define GND1 10
#define GND2 11
#define GND3 12
#define GND4 13

int sec = 0;         //  pulse counting clock
int mins = 0;
int tenmins = 0;
int hours = 8;
int tenhours = 0;
int clockState = 0;
int lastClock = 0;

pinMode (2,INPUT_PULLUP);    //    FAILS ON COMPILE  " expected constructor, destructor, or type conversion before '(' token"
//  pin 2 taking pulses from URTC AND TRIGGERING addSec
void edgeDet() {
    clockState = digitalRead(2);
    if(clockState << lastClock){   //   falling edge
        lastClock = clockState;
        addSec();   //   update tenhours, hours, tenmins, mins
   }
}

void addSec() {  //   Called by edgeDet
  sec=sec+1;
  if (sec > 59) {
     sec = 0;
     mins = mins + 1;
  }
     if (mins > 9) {
       mins = 0;
       tenmins = tenmins +1;
     }
     if (tenmins >5 ) {
         tenmins = 0;
         hours = hours +1;
     }
         if (hours > 9) {
           hours = 0;
           tenhours = tenhours + 1;
       }
       if (tenhours > 2) {
             tenhours = 0;
             hours = 0;
       }
  }    

//     use  functions below  to write tenhours to digit 1, hours to digit 2, tenmins to digit 3 and mins to digit 4

void digit0 () {         // functions to display digits 0 to 9 //  lights segments
digitalWrite(A, HIGH);
digitalWrite(B, HIGH);
digitalWrite(C, HIGH);
digitalWrite(D, HIGH);
digitalWrite(E, HIGH);
digitalWrite(f, HIGH);
digitalWrite(G, LOW);
};

void digit1 () {
digitalWrite(A,LOW);
digitalWrite(B, HIGH);
digitalWrite(C, HIGH);
digitalWrite(D, LOW);
digitalWrite(E, LOW);
digitalWrite(f, LOW);
digitalWrite(G, LOW);
};

void digit2 () {
digitalWrite(A,HIGH);
digitalWrite(B, HIGH);
digitalWrite(C, LOW);
digitalWrite(D, HIGH);
digitalWrite(E, HIGH);
digitalWrite(f, LOW);
digitalWrite(G, HIGH);
};

void digit3 () {
digitalWrite(A,HIGH);
digitalWrite(B, HIGH);
digitalWrite(C, HIGH);
digitalWrite(D, HIGH);
digitalWrite(E, LOW);
digitalWrite(f, LOW);
digitalWrite(G, HIGH);
};

void digit4 () {
digitalWrite(A,LOW);
digitalWrite(B, HIGH);
digitalWrite(C, HIGH);
digitalWrite(D, LOW);
digitalWrite(E, LOW);
digitalWrite(f, HIGH);
digitalWrite(G, HIGH);
};

void digit5 () {
digitalWrite(A,HIGH);
digitalWrite(B, LOW);
digitalWrite(C, HIGH);
digitalWrite(D, HIGH);
digitalWrite(E, LOW);
digitalWrite(f, HIGH);
digitalWrite(G, HIGH);
};

void digit6 () {
digitalWrite(A,HIGH);
digitalWrite(B, LOW);
digitalWrite(C, HIGH);
digitalWrite(D, HIGH);
digitalWrite(E, HIGH);
digitalWrite(f, HIGH);
digitalWrite(G, HIGH);
};

void digit7 () {
digitalWrite(A,HIGH);
digitalWrite(B, HIGH);
digitalWrite(C, HIGH);
digitalWrite(D, LOW);
digitalWrite(E, LOW);
digitalWrite(f, LOW);
digitalWrite(G, LOW);
};

void digit8 () {
digitalWrite(A, HIGH);
digitalWrite(B, HIGH);
digitalWrite(C, HIGH);
digitalWrite(D, HIGH);
digitalWrite(E, HIGH);
digitalWrite(f, HIGH);
digitalWrite(G, HIGH);

};

void digit9 () {
digitalWrite(A, HIGH);
digitalWrite(B, HIGH);
digitalWrite(C, HIGH);
digitalWrite(D, HIGH);
digitalWrite(E, LOW);
digitalWrite(f, HIGH);
digitalWrite(G, HIGH);

};

void choosedigit (int digit) {     //function to display digit from passed part of time
                                 //  instead of (int digit), I need (mins) , tenmins, hours, tenhours
                                 //   do I pass mins etc as digit  ??   And run 4 times  ??
  switch (digit) {

 case 0:
 digit0 ();
 break;

 case 1:
 digit1 ();
 break;

 case 2:
 digit2 ();
 break;

 case 3:
 digit3 ();
 break;

 case 4:
 digit4 ();
 break;

 case 5:
 digit5 ();
 break;

 case 6:
 digit6 ();
 break;

 case 7:
 digit7 ();
 break;

 case 8:
 digit8 ();
 break;

 case 9:
 digit9 ();    //   digit9 from digit0 (whichever) selects the pattern to be output to the segments for each digit
 break;

 default:

 break;

 }; 

};


void setup() {

Serial.begin (9600 );

pinMode(A, OUTPUT);
pinMode(B, OUTPUT);
pinMode(C, OUTPUT);
pinMode(D, OUTPUT);
pinMode(E, OUTPUT);
pinMode(f, OUTPUT);
pinMode(G, OUTPUT);

pinMode(GND1, OUTPUT);
pinMode(GND2, OUTPUT);
pinMode(GND3, OUTPUT);
pinMode(GND4, OUTPUT);

};
void loop () { 

   edgeDet();
   
  //   KEEP PRINTING THE TIME
 //  tenhours  //  most significant number  - check order
digitalWrite(GND1, HIGH);  // first digit on,
digitalWrite(GND2, LOW);   // others off
digitalWrite(GND3, LOW);
digitalWrite(GND4, LOW);
  choosedigit(tenhours);     //  calls digit for tens of hours that writes to bus
delay (1);


//  hours
digitalWrite(GND1, LOW); /   // first digit is off
digitalWrite(GND2, HIGH);    // second digit is on
  choosedigit(hours);        //  calls digit for hours to write to bus
delay (1); 

//   tenmins   
digitalWrite(GND2, LOW);
digitalWrite(GND3, HIGH);
    choosedigit(tenmins);    //  calls digit for tens of minutes
delay (1);

//   mins
digitalWrite(GND3, LOW); 
digitalWrite(GND4, HIGH);
    choosedigit(mins);      // digit for minutes
delay (1);

 }  //  loop back
