//   UnrealTimeClock   test clock only   28/04/20

//    code from (ClockHack) Clock Tick Demonstration   By Matt Mets, completed in 2008
//    with WAV player from SD card
//    via Panasonic TV IR Remote

 //   LEDs on pins 2 & 3 to register ticks   I see the flashes
 //   delay(1000) does the timing but doesn't count the 10ms pulses   --  make delay 990
 //   programme cycle time practically zero in stripped out form - monitor as code is added
 //   clock will probably stop while sounds play   -  does this matter ?

 //  SD and Remote stripped out   To reinstate it go back to UnrealTimeClock

// Compiles and runs in the right ballpark  28/04/20

//  next step - reinstate remote control  --  



int clockA = 2;          // Set these to the pin numbers you have attached the clock wires
int clockB = 3;          // to.  Order is not important. 
int tickPin = clockA;    // This keeps track of which clock pin should be fired next.
int pseudoSec = 990;    //  for normal clock
int timeShift = 10;      //  crank up clock speed


  
void setup() {

//   clock
  pinMode(clockB, OUTPUT);
  pinMode(clockA, OUTPUT);
  digitalWrite(clockB, LOW);
  digitalWrite(clockA, LOW);
}

// Move the second hand forward one position (one second on the clock face).

void doTick() {       //   a PROCEDURE
  
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

 delay(pseudoSec);
  
//  if (My_Receiver.GetResults(&My_Decoder)) {
//    My_Hash_Decoder.copyBuf(&My_Decoder);
//    My_Decoder.decode();
//    My_Hash_Decoder.decode();
//    Serial.println(My_Hash_Decoder.hash, DEC); 
//    My_Receiver.resume();
//  }

//    if (My_Hash_Decoder.hash == 193429999 ) { //   [<<   back to normal
//      pseudoSec = 990;
//      My_Hash_Decoder.hash = 0;
//}
 //   if (My_Hash_Decoder.hash == 780937865)  { //   ">>"   double speed
 //     pseudoSec = 495;
//      My_Hash_Decoder.hash = 0;
//    }  
//    if (My_Hash_Decoder.hash == 1307002951 ) { //   >>]  quad speed
//      pseudoSec = 247;
//      My_Hash_Decoder.hash = 0;
//}
//    if (My_Hash_Decoder.hash == 199473975) { //     "+"  x8 speed
//      pseudoSec = 125;
//      My_Hash_Decoder.hash = 0;
//}
//    if (My_Hash_Decoder.hash == 3571053223 ) { //   "-"  half speed
//      pseudoSec = 1980;
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
//     if (My_Hash_Decoder.hash == 2098629583 ) { //  0 
//            music.play("0.wav");      
//            My_Hash_Decoder.hash = 0;
 //    }           
 //     if (My_Hash_Decoder.hash == 4146609271) { //    1   sound 1
 //           music.play("1.wav");   
 //          My_Hash_Decoder.hash = 0;
 //    }
 //     if (My_Hash_Decoder.hash == 1971303647 ) { //   2  sound 2
 //             music.play("2.wav");   
 //             My_Hash_Decoder.hash = 0;
 //    }            
 //   if (My_Hash_Decoder.hash == 3007006103) { //    3  sound 3
//              music.play("3.wav");           //Play song 3 from start
 //             My_Hash_Decoder.hash = 0 ;                      
 //    }            
 //  if (My_Hash_Decoder.hash == 1006888199 ) { //   4   sound 4
 //             music.play("4.wav");   
 //             My_Hash_Decoder.hash = 0 ;  
 //    }           
 //   if (My_Hash_Decoder.hash == 3875886367 ) { //   5  sound 5
 //             music.play("5.wav");              
 //             My_Hash_Decoder.hash = 0 ;    
//     }            
 //   if (My_Hash_Decoder.hash == 2762313415 ) { //   6  sound 6
 //             music.play("6.wav");   
 //             My_Hash_Decoder.hash = 0 ;      
//      }            
//    if (My_Hash_Decoder.hash == 3806617471 ) { //   7  sound 7
 //             music.play("7.wav");   
 //             My_Hash_Decoder.hash = 0 ;    
 //     }           
 //   if (My_Hash_Decoder.hash == 1806499567 ) { //   8   sound 8
  //            music.play("8.wav");   
 //             My_Hash_Decoder.hash = 0 ;     
 //     }           
 //   if (My_Hash_Decoder.hash == 243853599) { //    9  sound 9
 //             music.play("9.wav");   
 //             My_Hash_Decoder.hash = 0 ;
 //    }
//              
//       }    
//    if (My_Hash_Decoder.hash == 4129172055 )  { //  top red power = E STOP
//         My_Hash_Decoder.hash = 0 ;     
//    }

        

//     Sounds from SD here 


}        // **********************************************************  LOOP AROUND
