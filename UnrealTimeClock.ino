//   UnrealTimeClock 2 bank sounds  Arduino Uno     Michael Beer

//  latest version  17/05/20    WORKING   *
//    needs PSU Gnd and Arduino Gnd commoned for relays to operate
//    330K resistors in series with amplifier inputs - direct from Arduino output
//    Relays now switch the power (12V) to the amplifiers
//    Volume up/down from remote not working.

//    Clock code from  Clock Tick Demonstration   By Matt Mets, completed in 2008
//    with WAV player from SD card    (basis) created  25 Jun 2017   by Aswinth Raj
//    https://circuitdigest.com/microcontroller-projects/arduino-audio-music-player
//    via Panasonic TV IR Remote from train controller code by Roger Miller
//    substituting IR Remote code from Simon Coward, due to problems with IRLib.h library
 //   LEDs on pins 2 & 3 to register ticks in place of clock; for now
 //    pin 7 output for Llanfair relay / 6 for Bethel relay  
 //   Serial monitor used for troubleshooting check speed once it's removed
 //   clock doesn't stop while sounds play  
//    clockCal from pot run on startup
 //   micro SD card loaded - 20 files 
//    Clock by remote control working -  looks close to accurate (as set at moment on pot) 
//    2 banks of sounds on same number keys  -  pin 5 LED indicates Bank 1 or 2
//    1 flash for bank 1 and 2 for bank 2  (3 for 3 even) 
//   IR Receiver pins   component side, pins downward -- L=Gnd, C=+V, R=Sig  //   in to pin 8

//  pins required for SD Card reader 
//  +5V to  Vcc
//  Gnd     Gnd
//  Pin 12  MISO (Master In Slave out)
//  Pin 11  MOSI (Master Out Slave In) 
//  Pin 13  SCK (Synchronous Clock)
//  Pin 4   CS (Chip Select)
//  Pin 9 to OUTPUT sound
//  Pin 10 set as output to prevent probs
//  according to Arduino on SPI.h   SS  (slave Select) should always be an output for SPI.h,  and on a UNO that's pin 10
//  WAV files 8 Bit Resolution ,   Sampling Rate 16000 Hz,   Audio Channel   Mono,   PCM format  PCM unsigned 8-bit

#include <TMRpcm.h>
#include <SD.h>
#include <SPI.h>
#define SD_chipSelectPin 4
TMRpcm music;

#include "IRremote.h"

int input_pin = 8; //set D8 as IR Remote input signal pin.
IRrecv irrecv(input_pin);
decode_results signals;
unsigned long ircodereceived; //ir code received, stored as unsigned long as some are very big number
//   IR Codes are specific for the Panasonic TV Remote that I am using
//   codes found using irrectest.ino  - origin unknown

int clockA = 2;         //  tick end of coil
int clockB = 3;         //  tock end of coil
int tickPin = clockA;   //  This keeps track of which clock pin should be fired next.
int pseudoSec = 900;    //  for normal clock - modified by pot
int clockCal = 1;       //  only cal on startup
int pot = 10;           //  mid range      pot on A0 pin
int clockFactor = 950;  //  calculated value of altered seconds
int Llanfair = 1;       //  relay on pin 7
int Bethel = 1;         //  relay on pin 6
int bank = 1;           //  sound bank switch
int volSet = 5;         //  volume 0 to 7

//   SD READER PINS      MOSI = 11; // MISO =12;  // SCK = 13;  // CS = 4;
 
void setup() {

//   SD audio
  pinMode (10, OUTPUT); //  disables unwanted mode
  music.speakerPin = 9; //   Audio out on pin 9  
  Serial.begin(9600);
  if (!SD.begin(SD_chipSelectPin)) {
    Serial.println("SD fail");
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
    switch(ircodereceived){
    }
    
//   clock
  pinMode(clockB, OUTPUT);   //  clock coil
  pinMode(clockA, OUTPUT);   //  clock coil other end
  digitalWrite(clockB, LOW);
  digitalWrite(clockA, LOW);
  pinMode(7,OUTPUT);         //  audio Llanfair relay
  pinMode(6,OUTPUT);         //  audio Bethel   relay
  pinMode(5,OUTPUT);         //  bank # indicator LED


}
  void doTick() {            //   a PROCEDURE
//   Move the second hand forward one position (one second on the clock face).
//   Energize the electromagnet in the correct direction.
    digitalWrite(tickPin, HIGH);
    delay(10);               //  10mS pulse
    digitalWrite(tickPin, LOW);  
// Switch the direction so it will fire in the opposite way next time.
    if (tickPin == clockA){
       tickPin = clockB;
    } else {
       tickPin = clockA;
  }
 }  //    end of void doTick(){

void loop() {

//  check calibration pot on startup only
   if (clockCal == 1){
      pot = analogRead(A0);
      pot = pot / 10;
//   could be smaller for bigger range - to start off
      pseudoSec = pseudoSec + pot;
      clockFactor - pseudoSec;
      clockCal = 0;   //  done once, don't do again
  }

 
    delay(clockFactor);
        doTick();
 
//      read Remote
   if (irrecv.decode(&signals)) {
      ircodereceived = signals.value;
      irrecv.resume(); // get the next signal
      switch(ircodereceived){

        case 3138312673:  //   cursor OK button   back to normal
           clockFactor = pseudoSec;
        break;

        case 3255005345: //   cursor up   double speed
          clockFactor = pseudoSec / 2;
        break;
 
        case 14188329544: //   Viera key       quad speed
          clockFactor = pseudoSec / 4;
        break;

        case 1307002951:    //     prog up   x8 speed
          clockFactor = pseudoSec / 8;
        break;

        case 2173897225: //   cursor down  half speed
          clockFactor = pseudoSec * 2;      
        break;

//   SOUND

         case 2553984193: //   CURSOR LEFT  Llanfair sound - toggle on/off
           if (Llanfair == 1) {
              digitalWrite(7, HIGH);
              Llanfair = 0;
           }
           else {
            if (Llanfair == 0 ) {
               digitalWrite(7, LOW);
               Llanfair = 1;
            }
           }
        break;

       case 553866289: //   CURSOR RIGHT  Bethel sound - toggle on/off
           if (Bethel == 1) {
              digitalWrite(6, HIGH);
              Bethel = 0;
           }
          else {
             if (Bethel == 0 ) {
               digitalWrite(6, LOW);
               Bethel = 1;
            }
           }
        break;

//  sound bank switching
     case 604803425: //  red key bank 1
         bank = 1;
         digitalWrite(5,HIGH);   //   LED 1 flash
         delay(100);
         digitalWrite(5,LOW);
     break;

     case 2724465097:  //  green key   bank 2
         bank = 2;
         digitalWrite(5,HIGH);  //   LED 2 flashes
         delay(100);
         digitalWrite(5,LOW);
         delay(100);
         digitalWrite(5,HIGH);
         delay(100);
         digitalWrite(5,LOW);
         delay(100);         
     break;
//  2724465097   for green key bank 3


       music.setVolume(volSet);   
               
       case 199473975:       //  vol up key             
          if (volSet < 7){
             volSet = volSet + 1;
             Serial.println(volSet);
          }
        break;
         case 3751053223:    //   vol up key
           if (volSet > 0){
              volSet = volSet - 1;
              Serial.println(volSet);
           }
     break;

     case 2860196199:        //   pause/play key
               music.pause();            
     break;
     
//  number keys 1 to 0  for sounds     1 to 20

     case 2098629583:    //   0     sound 10 / 20
           if (bank == 2){
              music.play("20.wav");
           } else {
              music.play("10.wav"); 
           }      
      break;
      
     case 4146609271:   //    1   sound 1 / 11
            if( bank == 2){
               music.play("11.wav");
            } else {
              music.play("1.wav");             
            }
     break;

     case 1971303647:   //   2  sound 2 / 12
           if (bank == 2){
              music.play("12.wav");
            }  else {
              music.play("2.wav");           
            }
     break;
           
    case 3007006103:    //    3  sound 3 / 13
          if (bank == 2){
              music.play("13.wav");
            }  else {
              music.play("3.wav");            
            }  
    break;                      
            
    case 1006888199:    //   4   sound 4 / 14
             if (bank == 2){
               music.play("14.wav");
            }  else {
               music.play("4.wav");             
            }
    break;  
           
    case 3875886367:   //   5  sound 5 /15
            if (bank == 2){
               music.play("15.wav");
            } else {
              music.play("5.wav");               
            }             
    break;    
          
    case 2762313415:    //   6  sound 6 / 16
           if (bank == 2){
              music.play("16.wav");
           }  else {
              music.play("6.wav");           
           }  
   break;     
           
    case 3806617471:   //   7  sound 7 / 17
           if (bank == 2){
             music.play("17.wav");
          }  else {
             music.play("7.wav");           
          }  
    break;    
          
    case 1806499567:   //   8   sound 8 / 18
          if (bank == 2){
             music.play("18.wav");
          }  else {
             music.play("8.wav");             
            }
 //   break;     
         
   case 243853599:      //  9  sound 9 / 19
          if (bank == 2){
            music.play("19.wav");
          }  else {
            music.play("9.wav");            
            }  
    break;
   default:
  break;  
    }  
  } 
 }
//   LOOP AROUND
