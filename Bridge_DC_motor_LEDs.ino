//  Bridge DC_motor_LEDs    from bridge code
//  code   MJB    -  to test using Motor 1 and Motor 2 for LED operateions   08/12/20
//   WORKS   15/01/21
//  using Arduino with LM293 motor shield
//  using MOTOR 1 to control signal  Motor 2 for alt flashing lights


//   steppercommand and servos commands inserted

//************  START WITH BRIDGE UP A BIT************************

#include <AFMotor.h>    //   stepper + LM293 + 2 servos

// signal by DCmotor 1  and nerner lights by DCmotor 2

#include <VarSpeedServo.h>
VarSpeedServo myservo1;
VarSpeedServo myservo2;


AF_Stepper stepmotor(200, 2);
int beep=0;

 AF_DCMotor DCmotor1(1);
 AF_DCMotor DCmotor2(2);


void setup() {

  
  stepmotor.setSpeed(10);
    DCmotor1.setSpeed(255);   // max glim
    DCmotor2.setSpeed(255);   // max glim

      // set up Serial library at 9600 bps
  Serial.begin(9600);
  //    delay(2000); while (!Serial);
  Serial.println("Run programme");
}
void loop () {


  beep = 0;
  while ( beep < 10) {
    DCmotor1.run(FORWARD);    //   signal GREEN
    delay (2000);
    DCmotor1.run(BACKWARD);    //  signal  RED 
    delay(2000);
    beep = beep + 1;

  }
      beep = 0;
  while ( beep < 20) {
    DCmotor2.run(FORWARD);    //  PLUS polarity to one red
    delay (500);
    DCmotor2.run(BACKWARD);    //   other RED on
    delay(500);
    beep = beep + 1;
    DCmotor2.run(RELEASE);  //   turn reds  off
  }


  
stepmotor.step(10, BACKWARD, MICROSTEP);    //  bridge down a bit
    stepmotor.release();



      myservo1.attach(9);             //  down gate
  myservo1.write (135, 5, true);  // to across track
  myservo1.detach();

  myservo2.attach(10);            //  up gate
  myservo2.write (45, 5, true);   //   to across track
  myservo2.detach();

    
  
}
