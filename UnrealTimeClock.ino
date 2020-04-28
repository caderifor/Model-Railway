//   UnrealTimeClock   
//    code from (ClockHack) Clock Tick Demonstration   By Matt Mets, completed in 2008
//    with WAV player from SD card
//    via Panasonic TV IR Remote
//
//   First draft  of gathered code  23/04/04
//   Michael Beer
//   SD Card code   created  25 Jun 2017   by Aswinth Raj
//   https://circuitdigest.com/microcontroller-projects/arduino-audio-music-player

//  pins required for SD Card reader 
//  +5V to  Vcc
//  Gnd     Gnd
//  Pin 12  MISO (Master In Slave out)
//  Pin 11  MOSI (Master Out Slave In)
//  Pin 13  SCK (Synchronous Clock)
//  Pin 4   CS (Chip Select)

//  WAV files
//  Bit Resolution  8 Bit
//  Sampling Rate 16000 Hz
//  Audio Channel   Mono
//  PCM format  PCM unsigned 8-bit

//  IR Connections  
//  S on IR to pin 11  //  ** change
//  - on IP to Pin GND  
//  remaining pin on IR to 5v

//   clock pins
//   extern unsigned long timer0_overflow_count;    //   what for ??
//   
#include <IRLib.h>
//  #include <SimpleSDAudio.h>    //   find and load

int RECV_PIN = 11;  // IR Reciever CLASH with SD  change pin !

//  int MOSI  = 11; 
//int MISO =12
//int SCK = 13
//int CS = 4
//  was pin 2 for play and 3 for pause  (not needed)  used by clock

// // to play track 3 (for example)
//  music.play("3.wav");

// // to pause
//   music.pause();

//  change the volume
// music.setVolume(5);        //Plays the song at volume 5


int clockA = 2;          // Set these to the pin numbers you have attached the clock wires
int clockB = 3;          // to.  Order is not important. 
int tickPin = clockA;    // This keeps track of which clock pin should be fired next.
int pseudoSec = 1000;    //  for normal clock
int timeShift = 10;      //  crank up clock speed


IRrecv My_Receiver(RECV_PIN);   //  see Clash above
IRdecode My_Decoder;
IRdecodeHash My_Hash_Decoder;


  
void setup() {
  // put your setup code here, to run once:

  //  IR Receive
  My_Receiver.enableIRIn(); // Start the receiver

//   SD audio
  music.speakerPin = 9; //Audio out on pin 9
  Serial.begin(9600); //Serial Com for debugging 
  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("SD fail");
    return;
  }

//  pinMode(2, INPUT_PULLUP); //  was Button 1 with internal pull up to chage track
//  pinMode(3, INPUT_PULLUP); //  was Button 2 with internal pull up to play/pause

music.setVolume(5);    //   0 to 7. Set volume level
music.quality(0);        //  Set 1 for 2x oversampling Set 0 for normal
//music.volume(0);        //   1(up) or 0(down) to control volume
//music.play("filename",30); plays a file starting at 30 seconds into the track    

//   clock
  pinMode(clockA, OUTPUT);
  pinMode(clockB, OUTPUT);
 
  digitalWrite(clockA, LOW);
  digitalWrite(clockB, LOW);
}

// Move the second hand forward one position (one second on the clock face).

void doTick() {       //   What is this about  ??   a subroutine ?? 
  // Energize the electromagnet in the correct direction.
  digitalWrite(tickPin, HIGH);
  delay(10);
  digitalWrite(tickPin, LOW);  
 
  // Switch the direction so it will fire in the opposite way next time.
  if (tickPin == clockA){
     tickPin = clockB;
  } else {
    tickPin = clockA;
  }
}  //    end of void doTick(){


void loop() {

  doTick();
  
  if (My_Receiver.GetResults(&My_Decoder)) {
    My_Hash_Decoder.copyBuf(&My_Decoder);
    My_Decoder.decode();
    My_Hash_Decoder.decode();
    Serial.println(My_Hash_Decoder.hash, DEC); 
    My_Receiver.resume();
  }

 //   if (My_Hash_Decoder.hash == 780937865)  { //   ">>"  button  
 //     do stuff
//      My_Hash_Decoder.hash = 0;
//    }  
//  if (My_Hash_Decoder.hash == 527303113 )  { //   "<<"   
//      stuff
//      My_Hash_Decoder.hash = 0;
//  }
//    if (My_Hash_Decoder.hash == 193429999 ) { //   [<<   back to normal
//      stuff
//      My_Hash_Decoder.hash = 0;
//}
//    if (My_Hash_Decoder.hash == 1307002951 ) { //   >>]  jump forward by  ?
//      stuff
//      My_Hash_Decoder.hash = 0;
//}
//    if (My_Hash_Decoder.hash == 199473975) { //     "+"  clock speedup
//      stuff
//      My_Hash_Decoder.hash = 0;
//}
//    if (My_Hash_Decoder.hash == 3571053223 ) { //   "-"  clock slowdown
//      stuff
//      My_Hash_Decoder.hash = 0;
//}
//    if (My_Hash_Decoder.hash == 438122687 ) { //   mute key  -  mute
//      stuff
//     My_Hash_Decoder.hash = 0;
//}
//    if (My_Hash_Decoder.hash == 4188329544) { //      viera  
//      stuff
//      My_Hash_Decoder.hash = 0;
//}
     if (My_Hash_Decoder.hash == 2098629583 ) { //  0 
            music.play("0.wav");      
            My_Hash_Decoder.hash = 0;
     }           
      if (My_Hash_Decoder.hash == 4146609271) { //    1   sound 1
            music.play("1.wav");   
           My_Hash_Decoder.hash = 0;
     }
      if (My_Hash_Decoder.hash == 1971303647 ) { //   2  sound 2
              music.play("2.wav");   
              My_Hash_Decoder.hash = 0;
     }            
    if (My_Hash_Decoder.hash == 3007006103) { //    3  sound 3
              music.play("3.wav");           //Play song 3 from start
              My_Hash_Decoder.hash = 0 ;                      
     }            
   if (My_Hash_Decoder.hash == 1006888199 ) { //   4   sound 4
              music.play("4.wav");   
              My_Hash_Decoder.hash = 0 ;  
     }           
    if (My_Hash_Decoder.hash == 3875886367 ) { //   5  sound 5
              music.play("5.wav");              
              My_Hash_Decoder.hash = 0 ;    
     }            
    if (My_Hash_Decoder.hash == 2762313415 ) { //   6  sound 6
              music.play("6.wav");   
              My_Hash_Decoder.hash = 0 ;      
      }            
    if (My_Hash_Decoder.hash == 3806617471 ) { //   7  sound 7
              music.play("7.wav");   
              My_Hash_Decoder.hash = 0 ;    
      }           
    if (My_Hash_Decoder.hash == 1806499567 ) { //   8   sound 8
              music.play("8.wav");   
              My_Hash_Decoder.hash = 0 ;     
      }           
    if (My_Hash_Decoder.hash == 243853599) { //    9  sound 9
              music.play("9.wav");   
              My_Hash_Decoder.hash = 0 ;
     }
//              
//       }    
//    if (My_Hash_Decoder.hash == 4129172055 )  { //  top red power = E STOP
//         My_Hash_Decoder.hash = 0 ;     
//    }

        

//     Sounds from SD here

//   if (digitalRead(2)==LOW  && debounce1 == true) {       //Button 1 Pressed

  song_number++;
  if (song_number==5){
      song_number=1;
   }
   //   'cos there is no song 5 ??
      debounce1=false;
      Serial.println("KEY PRESSED");
      Serial.print("song_number=");
      Serial.println(song_number);

//  presume I can use these lines in Remote button responses ??
  if (song_number ==1){
    music.play("1.wav",10);} //Play song 1 from 10th second 

  if (song_number ==2){
     music.play("2.wav",33);} //Play song 2 from 33rd second 

  if (song_number ==3){
    music.play("3.wav");} //Play song 3 from start

  if (song_number ==4){
     music.play("4.wav",25);} //Play song 4 from 25th second 

  if (digitalRead(3)==LOW  && debounce2 == true){      //Button 2 Pressed
     music.pause();  Serial.println("PLAY / PAUSE");
     debounce2=false;
  }
  if (digitalRead(2)==HIGH){    //Avoid debounce   //   There were no "{" on these lines ?
  debounce1=true;
}
  if (digitalRead(3)==HIGH){  //Avoid debounce     //   Will I need debounce for Remote calls ?
  debounce2=true;
}


}        // **********************************************************  LOOP AROUND
