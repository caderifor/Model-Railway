
//  Motor shield ( two LM293 chips) tester for Arduino Uno  
//  MJB  22/01/21

//  Stepper motor (M3 M4)  -  Nema 17
//  2 servos  -  pins 9 & 10 for servos
//  DCmotors M1 and M2

//  LED PAIRS ON M1, M2, M3 & M4 to indicate volts and polarity
//  each pair - diodes parallel but opposite polarity, in series with ballast resistor 
//   Servos
#include <VarSpeedServo.h>
VarSpeedServo myservo1;
VarSpeedServo myservo2;

//    Stepper
#include <AFMotor.h>   
AF_Stepper stepmotor(200, 2);

// DC motors
   AF_DCMotor DCmotor1(1);   
   AF_DCMotor DCmotor2(2);  

   int beep = 0;
   
void setup() {
     
     DCmotor1.setSpeed(126);   // max 
     DCmotor2.setSpeed(126);   // max 

     stepmotor.setSpeed(10);

// set up Serial library at 9600 bps
      Serial.begin(9600);
      delay(2000); while (!Serial);
      Serial.println("Run programme");
      Serial.println("Tests DC Motors 1 & 2, Stepper Motor on 1 and two servos");
}

void loop () {
    Serial.println("12V DC Motor 1 -  half speed");
    beep = 0;
    while(beep<10){
      Serial.println("             Forward");
      DCmotor1.run(FORWARD);    
      delay(300);      
      DCmotor1.run(RELEASE);
      delay(30);
      Serial.println("             Backward");
      DCmotor1.run(BACKWARD); 
      delay(300);
      DCmotor1.run(RELEASE);
      delay(30);
      beep = beep + 1;
    }
     DCmotor1.run(RELEASE); 

    Serial.println("12V DC Motor 2  -  half speed");
    beep = 0;
    while(beep<10){
      Serial.println("              Forward");
      DCmotor2.run(FORWARD); 
      delay(300);
      DCmotor2.run(RELEASE);
      delay(30);
      Serial.println("              Backward");      
      DCmotor2.run(BACKWARD);
      DCmotor2.run(RELEASE);
      delay(30);
      delay(300);
      beep = beep + 1;
    }
     DCmotor2.run(RELEASE); 

    Serial.println("stepper Motor (M3 M4) - speed 10");  
    beep = 0;
    while(beep<4){
      Serial.println("Stepper backward 50 Microstep");
      stepmotor.step(50, BACKWARD, MICROSTEP);  
      delay(100);
      Serial.println("Stepper forward 50 Microstep");
      stepmotor.step(50, FORWARD, MICROSTEP);
      delay(100);
      beep=beep +1;
    }
    stepmotor.release();
    delay(5000);
    

  Serial.println("Servo Motor 2");
  myservo1.attach(9);
  Serial.println("               MIDWAY");
  myservo1.write (90, 5, true);
  delay(300);
  Serial.println("               HIGH END (160)");
  myservo1.write (160, 5, true);
  delay(300);
  Serial.println("                LOW END (20)");
  myservo1.write (20, 5, true);  
  myservo1.detach();
    delay(300);


  Serial.println("Servo Motor 1");  
  myservo2.attach(10);
  Serial.println("               MIDWAY");  
  myservo2.write (90, 5, true);
  delay(300);
  Serial.println("               HIGH END (160)");
  myservo2.write (160, 5, true);
  delay(300);
  Serial.println("               LOW END (20)");
  myservo2.write (20, 5, true);  
  myservo2.detach();
  delay(300);
  Serial.println("LOOP BACK");

}  //  loop back
  
