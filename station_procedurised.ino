//   station crossing and auto door - Procedurised -
//   using servos and stepper motor  and stepper motor CD drive
//   with initialise routines for stepper and for servos
//   Modified 23/05/20  and 16/02/21   door open time by millis
//   Motor shield powered from 12V  9V regulator feeding Arduino jack.
//   loaded in rewired unit and not working   
//   WORKING again 12/04/21

//   PIR sensor  pin 2   Limit switch pin A0    Gate change button pin 13
//   gate angle values increase with anticlockwise rotation seen from above
//   **   door opens and closes continually if PIR sensor not positioned to its liking while in sunlight


unsigned long startMillis;
unsigned long currentMillis;
unsigned long period = 10000;

#include <VarSpeedServo.h>
VarSpeedServo myservo1;  //  pin9
VarSpeedServo myservo2;  //  pin 10
int gatepos = 2;   // 2 for passengers  -  1  for locos

#include <AFMotor.h>
AF_Stepper motor(200, 2);  //  motor shield port 2
int doorcheck = 2;   //  > 0  means not init yet
int gatescheck = 1;

//  PIR sensor
int inputPin = 2;  // PIR sensor pin 2
int pinval = 0;      //  analog reading for PIR detecting motion
int closed = 0;   //  door microswitch pulls A0 low and gives 1 for closed
int switchread = 0;   // microswitch read - analog range changed to 0 or 1

void setup() {
  Serial.begin(9600);

  motor.setSpeed(16);   //  door speed   (stepper)
  pinMode(13, INPUT);    //  pin 13 for change gates button
  pinMode(A0, INPUT);    //  ANALOG FOR DOOR CLOSED LIMIT SWITCH
  pinMode(2, INPUT);     //  PIR
}

void loop() {

  Serial.println("loop");

  Serial.println(doorcheck);
  if (doorcheck > 0) { // hasn't been initialised
    doorinit();    //    initialise door position
    Serial.println("initialise door");
  }
  Serial.println(gatescheck);
  if (gatescheck > 0) { // hasn't been initialised
    gatesinit();   //    initialise gates positions ?
  }

  //    NORMAL LOOP starts here

  if (closed == 1) {    //   open  door ?
    dooropen();      
  }

  if (closed == 0) {   //  close door ?
    doorclose();    
  }
    Serial.print("gatepos ");
        Serial.println(gatepos);
        delay (1000); 
  if (gatepos == 1) {
    gates2passengers();    //      gates to passengers ?
    Serial.println("gates to passengers");    
    delay (1000);   
  }
  else {
  if (gatepos == 2) {
    gates2rail();   //   gates to rail ?
    Serial.println("gates to trains");
    delay (1000);
  }
  }
}      // back to void loop



void doorinit() {
  switchread = (analogRead(A0));   //  1023 if open
  if (switchread > 200) {
    closed = 0;     //  not closed
  }
  while ( closed == 0) {
    motor.step(10, FORWARD, MICROSTEP);    //  door close a bit
    switchread = (analogRead(A0));
    if (switchread < 10) { //   microswitch closed
      closed = 1;
      motor.release();
      doorcheck = 0;
      //         delay(2000);  //  dont open door straight away
    }
  }
  Serial.println("door initialise done");
}   //   door has been initialised

void gatesinit() {

  myservo1.attach(9);                //     DOWN END GATE servo on pin 9
  myservo1.write(155, 5, true);     //  DOWN gate to setback
  myservo1.detach();

  myservo2.attach(10);               //     UP END GATE servo from  pin 10       -
  myservo2.write(135, 5, true);     //  UP gate  across track
  myservo2.detach();
  gatepos = 2;      //       servos now at passennger positio
  gatescheck = 0;
  Serial.println("gates initialise done");
}  //   gates have been initialised

void dooropen() {

  pinval = digitalRead(2);  //  read PIR input
  if (pinval == HIGH) {   //  check for input high
    motor.step(200, BACKWARD, MICROSTEP);  //   door open code
    motor.release();
    closed = 0;   // we just opened the door
    startMillis = millis();   // when we opened
  }
  Serial.println("door opened");
}    //   doors open

void doorclose() {
  currentMillis = millis();
  if (currentMillis - startMillis >= period) {  //   door has been open period millisec
    motor.step(200, FORWARD, MICROSTEP);    //  door close
    motor.release();
    closed = 1;
  }
  Serial.println("door closed");
}     //   door closed


void gates2rail() {
  if (digitalRead(13) == LOW) {       //  change button
    myservo1.attach(9);                //     DOWN END GATE servo on pin 9
    myservo2.attach(10);               //     UP END GATE servo from  pin 10       -
    //    to locos positions
    myservo1.write(155, 5, true);    //  DOWN gate to setback
    myservo2.write(10, 5, true);   //  UP gate to block passengers
    myservo1.write(45, 5, true);   //  DOWN gate in line with track
    myservo1.detach();
    myservo2.detach();
    gatepos = 1;             //   loco position
  }
  Serial.println("gates to rail");
}        //   gates in rail position

void gates2passengers() {
  if (digitalRead(13) == LOW) {       //  gate change button
    myservo1.attach(9);                //     DOWN END GATE servo on pin 9
    myservo2.attach(10);               //     UP END GATE servo from  pin 10       -
    //    to passengers positions
    myservo1.write(155, 5, true);     //  DOWN gate to setback
    myservo2.write(135, 5, true);     //  UP gate  across track
    myservo1.write(135, 5, true);     //  DOWN gate  across track
    myservo1.detach();
    myservo2.detach();
    gatepos = 2;      //       servos now at passennger position
  }
  Serial.println("gates to passengers");
}      //    gates to passengers
