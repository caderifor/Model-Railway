//  Grandads Garden Master
//  First combined code  not yet tested   23/06/25
//  COMPILES  25/06/25
//   via IR Remote     //  Servo stuff from pan n tilt
//  Based on controller 24 July 2017     working   :-)

// originally designed to use 5V PWM output on pin 9 through a 3302 voltage comparator to give 12V PWM
//  WHY NOT a MOSFET?


//  OUTPUT

//  pin 2   LED  CW red
//  pin 3   LED  CW green
//  pin 4   LED  CCW Red
//  pin 5   LED  CCW green
//  pin 6   Servo1
//  Pin 7   Servo2

//  pin 8 reverse relay   CW  or CCW
//  pin 9 motor drive analog PWM

//  INPUT
//  Pin 11
//  IR Rcv  [X]  pins l-3 -> code rec , GND, vcc

#include <IRLib.h>

int RECV_PIN = 11;

IRrecv My_Receiver(RECV_PIN);
IRdecode My_Decoder;
IRdecodeHash My_Hash_Decoder;

#include <VarSpeedServo.h>
VarSpeedServo myservo1;  //  pin6     Point1
VarSpeedServo myservo2;  //  pin7     Point2

int motorValue9 = 0;
int delaytime = 50 ;    //  untidy but is it a problem
int fwdbkwd9 = 1 ;
bool CW = LOW;
bool CCW = HIGH;
int count = 0;
int increment = 5;   //   could use to modify Step
int speedReq = 0;
int speedNow = 0;
int Step = 0;
int Code = 0;
bool Point1 = HIGH;   //   servos
int  Point1pos = 90;
bool Point2 = HIGH;
int  Point2pos = 90;
int servospeed = 10; //  set for whole sketch

void setup() {
  // setup code here, to run once:
  My_Receiver.enableIRIn(); // Start the receiver
  Serial.begin(9600);
  delay(2000); while (!Serial);

  pinMode(2, OUTPUT);   //  Sig1 red
  pinMode(3, OUTPUT);   //  Sig1 green
  pinMode(4, OUTPUT);   //  Sig2 green
  pinMode(5, OUTPUT);   //  Sig2 red
  pinMode(6, OUTPUT);    //   Servo1
  pinMode(7, OUTPUT);    //   Servo2
  pinMode(8, OUTPUT);   // reverse relay
  pinMode(9, OUTPUT);   // voltage output - analog
  //
  // set PWM freq for pin 9
  TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 divisor to     1 for PWM frequency of 31372.55 Hz
}

void loop() {

  Read();      //   IR commands
  Switch();    //   includes Points
  Signals();   //   LEDs set from points and direction
  Calc();      //   train speed

}

void Read() {
  if (My_Receiver.GetResults(&My_Decoder)) {
    My_Hash_Decoder.copyBuf(&My_Decoder);
    My_Decoder.decode();
    My_Hash_Decoder.decode();
    Serial.println(My_Hash_Decoder.hash, DEC);  //  optional
    Code = (My_Hash_Decoder.hash, DEC);
    My_Receiver.resume();
  }
}

void Switch() {   //   Enter correct codes
  switch (Code) {

    case  0000007: //   (ring) left arrow - CCW
      if (fwdbkwd9 == 1) {
        while (motorValue9 > 0) {           //  slow to zero before reversing
          analogWrite(9, motorValue9 - 1) ;
          motorValue9 = motorValue9 - 1 ;
        }
        fwdbkwd9 = 0 ;
      }
      break;

    case 0000006: //   Right arrow  CW   //   = forward
      if (fwdbkwd9 == 0) {
        while (motorValue9 > 0) {      //  slow to zero before reversing
          analogWrite(9, motorValue9 - 1) ;
          motorValue9 = motorValue9 - 1 ;
        }
        fwdbkwd9 = 1 ;
      }
      break;

    case 199473975: //  Up arrow   speed up
      Step = 5;
      break;

    case 000005: //     Down arrow slow down
      Step = -5;
      break;


    case 4129172055: //  top red power = E STOP
      analogWrite(9, 0);
      motorValue9 = 0 ;
      break;

    //   Perhaps do like latest CIR controller - start with Estop on and press Estop to reset


    //   POINTS

    case 000004: //   RED  Point 1 to straight
      myservo1.attach(6);                //    Point1 servo pin 6
      //    push rod  =  pull lever   =  105 degrees
      //    looked at from top of servo
      //    assuming control rod below pivot

      myservo1.write(75, servospeed, true);
      myservo1.detach();
      Point1 = HIGH;
      break;

    case 000003: //   GREEN  Point 1 to curved
      myservo1.attach(6);                //    Point1 servo pin 6
      myservo1.write(105, servospeed, true);
      myservo1.detach();
      Point1 = LOW;

      break;

    case 000002:       //   YELLOW  Point 2 to straight
      myservo2.attach(7);   //    Point2 servo pin 7
      myservo2.write(75, servospeed, true);
      myservo2.detach();
      Point2 = HIGH;
      break;

    case 000001: //   BLUE  Point 2 to curved
      myservo2.attach(7);                //    Point2 servo pin 7
      myservo2.write(105, servospeed, true);
      myservo2.detach();
      Point2 = LOW;
      break;
  }    //   end of switch
}

void Signals() {
  //  something like

  //  SIGNAL 1
  if ((Point1 = HIGH) && (CW = HIGH)) {
    digitalWrite(4, HIGH);  //   LED green on
    digitalWrite(5, LOW);   //   LED red off
  }

  if ((Point1 = HIGH) && (CW = LOW)) {
    digitalWrite(5, HIGH);  //   LED RED on
    digitalWrite(4, LOW);   //   LED green off
  }

  //  SIGNAL 2
  if ((Point2 = HIGH) && (CCW = HIGH)) {
    digitalWrite(6, HIGH);  //   LED green on
    digitalWrite(7, LOW);   //   LED red off
  }

  if ((Point2 = HIGH) && (CCW = LOW)) {
    digitalWrite(6, HIGH);  //   LED RED on
    digitalWrite(7, LOW);   //   LED green off
  }

}

void Calc() {
  //  calculate and drive routine
  //  Add set amount + or - to current speed
  // take in current speed
  speedNow = motorValue9;
  //   update speed this time around
  // Step   from switch ( + or -)
  speedReq = speedNow + Step;
  // speedReq must be between 0 and 255
  if (speedReq < 0) {
    speedReq = 0;
  }
  if (speedReq > 255 ) {
    speedReq = 255;
  }
  motorValue9 = speedReq;   //    output speed
  analogWrite(9, motorValue9 );
}


void Deadzone() {
  //   Not used ?

  if (speedReq < 80) {   //  skip deadzone and stop
    speedReq = 0;
    delaytime = 0;
  }
  while (motorValue9 > speedReq) {            //     wind down loop
    motorValue9 = motorValue9 - 1;
    analogWrite(9, motorValue9 );
    delay(delaytime);
  }

  //     if speeding up
  if (motorValue9 < speedReq) {
    if (speedReq < 6) {
      motorValue9 = 80;    //       skip deadzone and start
    }
    while (motorValue9 < speedReq) {         //       wind up loop
      motorValue9 = motorValue9 + 1 ;
      analogWrite(9, motorValue9);
      delay(delaytime);
    }
  }
}
