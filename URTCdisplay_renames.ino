//  URTC Display with routines renamed
//  triggered 4x7seg time display for URTC

//  4 x 7seg LED  readout of time from URTC nominally 1 sec pulses
//  27/08/20    formats but I am doubtful about passing of variables and stuff
//
//  built using code from "7 LED segments demo counter program.
//   https://www.electronicsblog.net/4-digits-7-segments-led-display-multiplexing-with-arduino/


//    pin 2 clock pulse in, read by edgeDet catching a falling edge
//    calls     procedure addSec()  to update time


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
void loop() {


  edgeDet();    //  calls addsec

  //   KEEP PRINTING THE TIME
  lightChip1();
  lightChip2();
  lightChip3();
  lightChip4();

}  //  loop back

void edgeDet() {
  //  pin 2 taking pulses from URTC AND TRIGGERING addSec
  clockState = digitalRead(2);
  if (clockState << lastClock) { //   falling edge
    lastClock = clockState;
    addSec();   //   update tenhours, hours, tenmins, mins
  }
}

void addSec() {
  //   Called by edgeDet
  sec = sec + 1;
  if (sec > 59) {
    sec = 0;
    mins = mins + 1;
  }
  if (mins > 9) {
    mins = 0;
    tenmins = tenmins + 1;
  }
  if (tenmins > 5 ) {
    tenmins = 0;
    hours = hours + 1;
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
void chooseVal (int unit) {
  //  where unit is tenhours, hours, tenmins or mins
  //   function to display select glyph from each passed unit of time

  switch (unit) {

    case 0:
      glyph0 ();     //   digit0 to digi9  selects the pattern to be output to the segments for each digit
      break;

    case 1:
      glyph1 ();
      break;

    case 2:
      glyph2 ();
      break;

    case 3:
      glyph3 ();
      break;

    case 4:
      glyph4 ();
      break;

    case 5:
      glyph5 ();
      break;

    case 6:
      glyph6 ();
      break;

    case 7:
      glyph7 ();
      break;

    case 8:
      glyph8 ();
      break;

    case 9:
      glyph9 ();
      break;

    default:

      break;

  };

};

//     use  functions below  to write tenhours to digit 1, hours to digit 2, tenmins to digit 3 and mins to digit 4

void glyph0 () {
  // functions to display glyphs 0 to 9 //  lights segments
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(G, LOW);
}

void glyph1 () {
  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(f, LOW);
  digitalWrite(G, LOW);
}

void glyph2 () {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, LOW);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(f, LOW);
  digitalWrite(G, HIGH);
}

void glyph3 () {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, LOW);
  digitalWrite(f, LOW);
  digitalWrite(G, HIGH);
}

void glyph4 () {
  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(f, HIGH);
  digitalWrite(G, HIGH);
}

void glyph5 () {
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, LOW);
  digitalWrite(f, HIGH);
  digitalWrite(G, HIGH);
}

void glyph6 () {
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(G, HIGH);
}

void glyph7 () {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(f, LOW);
  digitalWrite(G, LOW);
};

void glyph8 () {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(G, HIGH);

};

void glyph9 () {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, LOW);
  digitalWrite(f, HIGH);
  digitalWrite(G, HIGH);

};

void lightChip1() {
  //  tenhours  //  most significant number
  digitalWrite(GND2, LOW);    // 3 chips off
  digitalWrite(GND3, LOW);
  digitalWrite(GND4, LOW);
  digitalWrite(GND1, HIGH);   // activate tenhours chip
  chooseVal(tenhours);        // call value for tens of hours that writes to bus
  delay (1000);
}
void lightChip2() {
  //  hours
  digitalWrite(GND1, LOW);    // chip is off
  digitalWrite(GND3, LOW);
  digitalWrite(GND4, LOW);
  digitalWrite(GND2, HIGH);   //   activate hours chip
  chooseVal(hours);           //  calls value for hours to write to bus
  delay (1000);
}
void lightChip3() {
  //   tenmins
  digitalWrite(GND1, LOW);
  digitalWrite(GND2, LOW);
  digitalWrite(GND4, LOW);
  digitalWrite(GND3, HIGH);   //  activate tenmins chip
  chooseVal(tenmins);         //  calls value for tens of minutes
  delay (1000);
}
void lightChip4() {
  //   mins
  digitalWrite(GND1, LOW);
  digitalWrite(GND2, LOW);
  digitalWrite(GND3, LOW);
  digitalWrite(GND4, HIGH);   //  activate mins chip
  chooseVal(mins);            // value for minutes
  delay (1000);
}
