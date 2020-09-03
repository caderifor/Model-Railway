
//   UnrealDigiTimeClock up to 4 bank sounds, 3 Amps, o/p to digi clock  Arduino Uno     Michael Beer

//    03/09/20  compiles
//    10mS pulses out (pin 2) to drive display   //   100R resistor from pin 2 to pin 2
//    Timing of pseudo seconds and pulse length read using millis and not delay

//    needs PSU Gnd and Arduino Gnd commoned for relays to operate
//    330K resistors in series with amplifier inputs  from Arduino output

//    with WAV player from SD card    (basis) created  25 Jun 2017   by Aswinth Raj
//    https://circuitdigest.com/microcontroller-projects/arduino-audio-music-player

//    via Panasonic TV IR Remote from train controller based on idea by Roger Miller
//    substituting IR Remote code from Simon Coward, due to problems with IRLib.h library
//    pin 7 output for Llanfair relay / 6 for Bethel relay / pin 3 for midway relay
//    micro SD card loaded - 30 files so far
//    Pin 9 to OUTPUT sound to all 3 amps
//    up to 4 banks of sounds on colour keys  -  pin 5 LED flashes to indicate Bank number
//    IR Receiver pins   component side, pins downward -- L=Gnd, C=+V, R=Sig  //   in to pin 8

//  pins required for SD Card reader
//  +5V to  Vcc
//  Gnd     Gnd
//  Pin 12  MISO (Master In Slave out)
//  Pin 11  MOSI (Master Out Slave In)
//  Pin 13  SCK (Synchronous Clock)
//  Pin 4   CS (Chip Select)
//  Pin 10 set as output to prevent probs
//  according to Arduino on SPI.h   SS  (slave Select) should always be an output for SPI.h,  and on a UNO that's pin 10
//  WAV files 8 Bit Resolution ,   Sampling Rate 16000 Hz,   Audio Channel   Mono,   PCM format  PCM unsigned 8-bit
//  Accurate timekeeping using millis
unsigned long startMillis;
unsigned long currentMillis;
unsigned long period = 1000;
unsigned long endTickCheck;

#include <TMRpcm.h>
#include <SD.h>
#include <SPI.h>
#define SD_chipSelectPin 4
TMRpcm music;
//   SD READER PINS      MOSI = 11; // MISO =12;  // SCK = 13;  // CS = 4;

#include "IRremote.h"

int input_pin = 8; //set D8 as IR Remote input signal pin.
IRrecv irrecv(input_pin);
decode_results signals;
unsigned long ircodereceived; //ir code received, stored as unsigned long as some are very big number
//   IR Codes are specific for the Panasonic TV Remote that I am using
//   codes found using irrectest.ino  - origin unknown

int centre = 1;         //  relay on pin 3
int Llanfair = 1;       //  relay on pin 7
int Bethel = 1;         //  relay on pin 6
int bank = 1;           //  sound bank switch

void setup() {
  //   SD audio
  pinMode (10, OUTPUT); //  disables unwanted mode
  music.speakerPin = 9; //   Audio out on pin 9

  if (!SD.begin(SD_chipSelectPin)) {
    return;
  }
  delay(2000);
  while (!Serial);

  //  IR setup
  irrecv.enableIRIn();   //    enable input from IR Receiver
  if (irrecv.decode(&signals)) {
    ircodereceived = signals.value;
  }
  irrecv.resume();      // get the next signal
  switch (ircodereceived) {
  }

  pinMode(2, OUTPUT);        //  clock pulse out
  pinMode(3, OUTPUT);        //  audio central
  pinMode(7, OUTPUT);        //  audio Llanfair relay
  pinMode(6, OUTPUT);        //  audio Bethel   relay
  pinMode(5, OUTPUT);        //  bank # indicator LED

  digitalWrite(3, HIGH);  //  AMPLIFIERS OFF
  digitalWrite(6, LOW);
  digitalWrite(7, HIGH);
}

void loop() {

  // check if due for a tick by reading millis
  currentMillis = millis();
  if ((currentMillis - startMillis) >= period) {
    digitalWrite(2, HIGH);
    startMillis = currentMillis;
  }
  //  time to end clock pulse ?
  endTickCheck = millis();
  if ( endTickCheck >= (startMillis + 10)) {
    digitalWrite(2, LOW);
  }

  //      read Remote
  if (irrecv.decode(&signals)) {
    ircodereceived = signals.value;
    irrecv.resume(); // get the next signal

    switch (ircodereceived) {

      case 3138312673:  //   cursor OK button - normal speed
        period = 1000; // one second
        break;

      case 3255005345: //   cursor up   4x speed
        period = 250;
        break;

      case 4188329544: //   Viera key       16x speed
        period = 62;
        break;

      case 2508605914:  //   ASPECT key   64x speed
        period = 16;
        break;

      case 1873753284:   //   + 3D key    250x speed
        period = 4;
        break;

      case 2173897225: //   cursor down  half speed
        period = 2000;
        break;

      case 4129172055: //   POWER RED  stop (almost)
        period = 100000;
        break;


      //   SOUND

      case 2553984193: //   CURSOR LEFT  Llanfair sound
        digitalWrite(7, LOW);

        break;

      case 553866289: //   CURSOR RIGHT  Bethel sound
        digitalWrite(6, HIGH);

        break;


      case 2251520594: //   OPTION KEY   centre sound
        digitalWrite(3, LOW);

        break;

      case 438122687: //   MUTE KEY  all 3 sound off
        digitalWrite(3, HIGH);
        digitalWrite(6, LOW);
        digitalWrite(7, HIGH);

        break;

      //  sound bank switching
      case 604803425:   //  red key bank 1
        bank = 1;
        digitalWrite(5, HIGH);  //   LED 1 flash
        delay(200);
        digitalWrite(5, LOW);
        delay(800);
        break;

      case 2724465097:  //  green key   bank 2
        bank = 2;
        digitalWrite(5, HIGH); //   LED 2 flashes
        delay(200);
        digitalWrite(5, LOW);
        delay(200);
        digitalWrite(5, HIGH);
        delay(200);
        digitalWrite(5, LOW);
        delay(800);
        break;

      case 3760167553:  //  yellow key   bank 3
        bank = 3;
        digitalWrite(5, HIGH); //   LED 3 flashes
        delay(200);
        digitalWrite(5, LOW);
        delay(200);
        digitalWrite(5, HIGH);
        delay(200);
        digitalWrite(5, LOW);
        delay(200);
        digitalWrite(5, HIGH);
        delay(200);
        digitalWrite(5, LOW);
        delay(800);
        break;


      case 1760049649:     //   blue key    bank 4
        bank = 4;
        digitalWrite(5, HIGH); //   LED 4 flashes
        delay(200);
        digitalWrite(5, LOW);
        delay(200);
        digitalWrite(5, HIGH);
        delay(200);
        digitalWrite(5, LOW);
        delay(200);
        digitalWrite(5, HIGH);
        delay(200);
        digitalWrite(5, LOW);
        delay(200);
        digitalWrite(5, HIGH);
        delay(200);
        digitalWrite(5, LOW);
        delay(800);
        break;

      //  number keys 1 to 0  for sounds 1 to 10 in each of 3-4 banks

      case 2098629583:    //   0     sound 10 / 20 / 30 / 40?
        if ( bank == 1) {
          music.play("10.wav");
        }
        if ( bank == 2) {
          music.play("20.wav");
        }
        if (bank == 3) {
          music.play("30.wav");
        }
        if (bank == 4) {
          music.play("40.wav");
        }
        break;

      case 4146609271:   //    *1   sounds
        if ( bank == 1) {
          music.play("1.wav");
        }
        if ( bank == 2) {
          music.play("11.wav");
        }
        if (bank == 3) {
          music.play("21.wav");
        }
        if (bank == 4) {
          music.play("31.wav");
        }
        break;

      case 1971303647:   //   *2  sound s
        if ( bank == 1) {
          music.play("2.wav");
        }
        if ( bank == 2) {
          music.play("12.wav");
        }
        if (bank == 3) {
          music.play("22.wav");
        }
        if (bank == 4) {
          music.play("32.wav");
        }
        break;

      case 3007006103:    //    *3  sounds
        if ( bank == 1) {
          music.play("3.wav");
        }
        if ( bank == 2) {
          music.play("13.wav");
        }
        if (bank == 3) {
          music.play("23.wav");
        }
        if (bank == 4) {
          music.play("33.wav");
        }
        break;

      case 1006888199:    //   *4   sounds
        if ( bank == 1) {
          music.play("4.wav");
        }
        if ( bank == 2) {
          music.play("14.wav");
        }
        if (bank == 3) {
          music.play("24.wav");
        }
        if (bank == 4) {
          music.play("34.wav");
        }
        break;

      case 3875886367:   //   *5  sounds
        if ( bank == 1) {
          music.play("5.wav");
        }
        if ( bank == 2) {
          music.play("15.wav");
        }
        if (bank == 3) {
          music.play("25.wav");
        }
        if (bank == 4) {
          music.play("35.wav");
        }
        break;

      case 2762313415:    //   *6  sounds
        if ( bank == 1) {
          music.play("6.wav");
        }
        if ( bank == 2) {
          music.play("16.wav");
        }
        if (bank == 3) {
          music.play("26.wav");
        }
        if (bank == 4) {
          music.play("36.wav");
        }
        break;

      case 3806617471:   //   *7  sounds
        if ( bank == 1) {
          music.play("7.wav");
        }
        if ( bank == 2) {
          music.play("17.wav");
        }
        if (bank == 3) {
          music.play("27.wav");
        }
        if (bank == 4) {
          music.play("37.wav");
        }
        break;

      case 1806499567:   //   *8   sounds
        if ( bank == 1) {
          music.play("8.wav");
        }
        if ( bank == 2) {
          music.play("18.wav");
        }
        if (bank == 3) {
          music.play("28.wav");
        }
        if (bank == 4) {
          music.play("38.wav");
        }
        break;

      case 243853599:      //  *9  sounds
        if ( bank == 1) {
          music.play("9.wav");
        }
        if ( bank == 2) {
          music.play("19.wav");
        }
        if (bank == 3) {
          music.play("29.wav");
        }
        if (bank == 4) {
          music.play("39.wav");
        }
        break;
      default:
        break;
    }
  }
}     //   LOOP AROUND
