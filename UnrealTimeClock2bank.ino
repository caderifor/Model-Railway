//   UnrealTimeClock 2 bank sounds  09/05/20     Michael Beer

//    Clock code from  Clock Tick Demonstration   By Matt Mets, completed in 2008
//    with WAV player from SD card    (basis) created  25 Jun 2017   by Aswinth Raj
//    https://circuitdigest.com/microcontroller-projects/arduino-audio-music-player
//    via Panasonic TV IR Remote from train controller code by Roger Miller
//    substituting IR Remote code from Simon Coward, due to problems with IRLib.h library
 //   LEDs on pins 2 & 3 to register ticks in place of clock; for now
 //    pin 7 output for Llanfair / Bethel relay
 //   Serial monitor used for troubleshooting check speed once it's removed
 //   clock will probably stop while sounds play   -  does this matter ?
//    clockCal from pot run on startup
 //   SD  files not ready

//   03/05/20    timing by delay replaced didn't work   delay(clockFactor) does
//   Clock by remote control working -  Sound not ready yet
//   Toggle pin 7 from RED button   WORKS
//    2 banks of sounds on same number keys  -  pin 5 LED indicates Bank 2

//   IR Receiver pins   component side, pins downward -- L=Gnd, C=+V, R=Sig  to pin 8

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

//  WAV files
//  Bit Resolution  8 Bit
//  Sampling Rate 16000 Hz
//  Audio Channel   Mono
//  PCM format  PCM unsigned 8-bit

#include <TMRpcm.h>
#include <SD.h>
#include <SPI.h>
#define SD_chipSelectPin 4
TMRpcm music;

#include "IRremote.h"

int input_pin = 8; //set D8 as input signal pin.
IRrecv irrecv(input_pin);
decode_results signals;
unsigned long ircodereceived; //ir code received, stored as unsigned long as some are very big number

int clockA = 2;          // tick end of coil
int clockB = 3;          // tock end of coil
int tickPin = clockA;    // This keeps track of which clock pin should be fired next.
int pseudoSec = 900;    //  for normal clock - modified by pot
int clockCal = 1;       //  only cal on startup
int pot = 10;           //  mid range
//  pot on A0 pin
int clockFactor = 950;  // calculated value of altered seconds
int Llanfair = 1;  //   relay on pin 7
int Bethel = 1;    //   relay on pin 6
int bank = 1;      //   sound bank switch

//   these 4 lines cause "conflicting declaration"
// can't see where I got this line from.  Perhaps they were just notes.
// perhaps they are declared in the library
//int MOSI = 11; 
//int MISO =12;
//int SCK = 13;
//int CS = 4;
 
void setup() {
  
  Serial.begin(9600);
  delay(2000); 
  while (!Serial); 
  
 //  IR setup 
    irrecv.enableIRIn();   //    enable input from IR Receiver
    if (irrecv.decode(&signals)) {
    ircodereceived = signals.value;
//    Serial.println(ircodereceived);
    }   
    irrecv.resume(); // get the next signal
    switch(ircodereceived){
    }
//   clock
  pinMode(clockB, OUTPUT);  
  pinMode(clockA, OUTPUT);
  digitalWrite(clockB, LOW);
  digitalWrite(clockA, LOW);
  pinMode(7,OUTPUT);   //  audio Llanfair
  pinMode(8,OUTPUT);   //  audio Bethel
  pinMode(5,OUTPUT);   //  bank 2 indicator

//   SD audio
  pinMode (10, OUTPUT);
  music.speakerPin = 9; //   Audio out on pin 9  
  if (!SD.begin(SD_chipSelectPin)) {
    Serial.println("SD fail");
    return;
  }
}
  void doTick() {      //   a PROCEDURE
  // Move the second hand forward one position (one second on the clock face).
  // Energize the electromagnet in the correct direction.
    digitalWrite(tickPin, HIGH);
    delay(10);       //  10mS pulse
    digitalWrite(tickPin, LOW);  
   // Switch the direction so it will fire in the opposite way next time.
  if (tickPin == clockA){
     tickPin = clockB;
  } else {
    tickPin = clockA;
  }
 }  //    end of void doTick(){

void loop() {

//  check calibration pot on startup
   if (clockCal == 1){
     pot = analogRead(A0);
//           Serial.println(pot);
     pot = pot / 10;
//   could be smaller for bigger range - to start off
              Serial.println(pot);
     pseudoSec = pseudoSec + pot;
     clockFactor - pseudoSec;
     clockCal = 0;
//         Serial.println(pseudoSec);
  }

 
    delay(clockFactor);
        doTick();
 
//      read Remote
  if (irrecv.decode(&signals)) {
      ircodereceived = signals.value;
      irrecv.resume(); // get the next signal
      switch(ircodereceived){

        case 527303113:  //   <<   back to normal
           clockFactor = pseudoSec;
        break;

        case 780937865: //   ">>"   double speed
          clockFactor = pseudoSec / 2;
        break;
 
        case 1307002951: //   up arrow       quad speed
          clockFactor = pseudoSec / 4;
        break;

        case 199473975:    //     "+"  x8 speed
          clockFactor = pseudoSec / 8;
        break;

        case 3571053223: //   "-"  half speed
          clockFactor = pseudoSec * 2;      
        break;

//   SOUND
//   successful test of toggle speakers on / off
//   ***   may need to reverse sense of outputs to suit relay modules

//   music.pause();  // to pause    from mute key?   or  bottom row ][ pause key ?
//   will key be read while music is playing ??
//    code for mute = ?????????


         case 604803425: //   RED  Llanfair sound
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

       case 1760049649: //   BLUE  Bethel sound
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

      case 4129172055:  { //  top red power = BANK SWITCH   LED for bank 2
        Serial.println("  red  power button");
           if (bank == 1) {
              bank = 2;
              digitalWrite(5,HIGH);
          } else {
          if (bank == 2 ){
              bank = 1;
              digitalWrite(5,LOW);
        }
      }
//   break;

     case 2098629583: //   0     sound 0 / 10
           if (bank == 2){
              music.play("10.wav");
    Serial.println("10.wav");
           } else {
              music.play("0.wav"); 
    Serial.println("0.wav");
           }      
      break;
      
     case 4146609271: //    1   sound 1 / 11
            if( bank == 2){
               music.play("11.wav");
            } else {
              music.play("1.wav");             
            }
     break;

     case 1971303647: //   2  sound 2 / 12
           if (bank == 2){
              music.play("12.wav");
            }  else {
              music.play("2.wav");           
            }
     break;
           
    case 3007006103: //    3  sound 3 / 13
          if (bank == 2){
              music.play("13.wav");
            }  else {
              music.play("3.wav");            
            }  
    break;                      
            
    case 1006888199: //   4   sound 4
             if (bank == 2){
               music.play("14.wav");
            }  else {
               music.play("4.wav");             
            }
    break;  
           
    case 3875886367: //   5  sound 5 /15
            if (bank == 2){
               music.play("15.wav");
            } else {
              music.play("5.wav");               
            }             
    break;    
          
    case 2762313415:  //   6  sound 6 / 16
           if (bank == 2){
              music.play("16.wav");
           }  else {
              music.play("6.wav");           
           }  
   break;     
           
    case 3806617471: //   7  sound 7 / 17
           if (bank == 2){
             music.play("17.wav");
          }  else {
             music.play("7.wav");           
          }  
    break;    
          
    case 1806499567://   8   sound 8 / 18
          if (bank == 2){
             music.play("18.wav");
          }  else {
             music.play("8.wav");             
            }
 //   break;     
         
   case 243853599:  //    9  sound 9 / 19
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
}
//   LOOP AROUND
