//  PWM train controller with multi simulation of driving a steam locomotive
//  CaderIfor by Michael Beer
//  24/06/21  WORKING   :-)

//  modes  SIMPLE or   COMPLICATED - with regulator, gear, brake and train weight

//  08/05/21    compiles   reading inputs works (1 pot and fixed variables)
//  09/05/21    latching alarms worked out   compiles
//  16/05/21    Brake release  -  OFF not bottom of track
//              TRIP reset built into LOOP - for if system tripped
//   21/05/21   "simple"  switch  read in first loop on startup
//   24/05/21   Wheel Slip  (LED pin7) + (hardware switch for +5 or USB)
//   28/05/21   ES proc done - compiles
//   31/05/21   alarm sets and resets in main loop    
//   03/06/21   Brakes ratchet works   is Brake reset fixed ?
//   07/06/21   Slip modified with slipping flag
//   09/06/21   float variables for regulator and gear
//   12/06/21   added Reverse gear to SIMPLE -  
//   13/06/21   Gear deadzone LED for "simple"  faster cycle time
//   22/06/21   Estop works - looks for press and release
//   23/06/21   Brake reset allows drive to pop up after releasing
//   24/06/21   Fixed by removing part of Brake in Pots

//   5V PWM output on pin 9   give 12V PWM via H Bridge to the meters and track  WORKS.
//   Hbridge control hardwired one direction
//   maximised PWM Speed for pin 9

float regulator = 0;     //  reg * gear overflows int variable
float gear = 0;
float speedUp = 0;
int motorValue = 0;      //  final output pwm value
int brake = 0;           //  to calculate
int brakePotNow = 0;     //  read pot
int brakePotPrev = 0;    //  last time around
int brakeRelease = 100;  //  release rate calculated
int train = 0;           //  weight sim
int speedDown = 0;
int acceleration = 0;
int FwdRev = 0;
bool slipping = LOW;     //  High if slipping detected
bool Estop = HIGH;
bool esLatch = LOW;
bool esButton = HIGH;
bool TRIP = HIGH;        //  HIGH means TRIP has occurred
bool Simple = LOW;       //  Read switch
bool Modeinit = LOW;     //  check simple once only

void setup() {

  pinMode(2, OUTPUT);   //   LED for Estop
  pinMode(3, INPUT);    //   Estop  button
  pinMode(4, OUTPUT);   //   Simple LED
  pinMode(5, INPUT);    //   SIMPLE switch
  pinMode(6, OUTPUT);   //   Mid Gear LED
  pinMode(7, OUTPUT);   //   REVERSE relay
  pinMode(9, OUTPUT);   //   Train PWM
  pinMode(10, OUTPUT);  //   brake force LED pwm
  pinMode(11, OUTPUT);  //   SLIP LED 
  pinMode(13, INPUT);   //   Overcurrent read

  //  A0 = train pot
  //  A2 = Brake pot
  //  A4 = regulator pot
  //  A5 = Gear pot

  Serial.begin(9600);

  // set PWM freq for pin 9
  TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 divisor to     1 for PWM frequency of 31372.55 Hz
}

void loop() {

  if (Modeinit == LOW) {  //   check mode switch at startup only
    Simple = digitalRead(5);
    digitalWrite(4, Simple);          //  simple led
    Modeinit = HIGH;
  }

  //   start of loop from second time onwards
  if (TRIP == HIGH) {
    digitalWrite(2, HIGH);  //  ESTOP RED LED ON //  if tripped, is it ok to reset now??
    EstopReset ();
  }

  if (Simple == HIGH) {     //  set on startup (above)
    simple();
  }
  else {
    complicated();
  }
  EstopSet();

  delay(100);    //   set loop speed / faster time constant
}    //  LOOP AROUND

void simple() {
  gear = (analogRead(A5));     //  from pots
  if (gear > 512) {
    digitalWrite(7, LOW);      //   Reverse relay off
  }
  else   {
    if (gear < 512) {
      digitalWrite( 7, HIGH);   //   Reverse relay on
    }
    gear = (gear - 512) / 2;    //   255 max
      FwdRev = (512 - gear) /2;  //  255 max
  if (FwdRev < 20) {
    digitalWrite(6 , HIGH);     //  dead zone  LED
  }
  else {
    digitalWrite(6 , LOW);
  }
    } 
     regulator = (analogRead(A4));
  regulator = (regulator / 5);    //   max < 255
  if (esLatch == HIGH ) { //  no probs
    analogWrite(9, regulator) ;
  }
}                          //  ends simple

void complicated() {
  digitalWrite(4, LOW);    //   not simple
  pots();
  calculate();
  motorDrive();
}    //  endof Complicated

void pots() {      //   read values

  regulator = (analogRead(A4));
  regulator = (regulator / 5);
  brakePotNow  = (analogRead(A2));
  if ( brakePotNow > brakePotPrev) {  //  pushing up
    brake = brakePotNow;              //  use new value
  }   else  {
    brake = brakePotPrev;             //  stay same - ignore reduction
  }
  brakePotNow  = (analogRead(A2));
  if (brakePotNow < 100) {            //  knob pulled down to "Release"
    brakeRelease = (100 - brakePotNow);
  }  else  {
    brakeRelease = 0;
  }
 // brake = brake - brakeRelease;
//  brakePotPrev = brake;               //     for next time
 // if (brake < 0) {
  //  brake = 0;
  //}                                   //  thus far 0 to 1024
  brake = (brake / 4);                //0 to 255 approx -> calculations
  if (( motorValue > 80) && (brake < 1)) { //  bowling along
    brake = 10;                       //  simulate friction
  }
  if (brake > 30) {
    analogWrite (10 , brake);         //   proportional LED
  } else {
    analogWrite (10 , 0); 
  }
  train = (analogRead(A0));           // weight of train
  train = (train / 35);
  if (train < 10) {
    train = 10;                       //   light engine
  }

  gear = (analogRead(A5));   
  if (gear > 512) {
    digitalWrite(7, LOW);            //   Reverse relay off
    gear = (gear - 512) / 2;         //   255 max
  }
  else   {
    if (gear < 512) {
      digitalWrite( 7, HIGH);        //   Reverse relay on
      gear = (512 - gear) / 2;       //  255 max
    }
  }
  if (gear < 20) {
    digitalWrite(6 , HIGH);          //  dead zone  LED
    gear = 0;                        //  dead zone / mid gear
  }
  else {
    digitalWrite(6 , LOW);
  }
}    //   end pots

void calculate() {
  if ((motorValue == 0) && (brake > 20)) {
    brake = brake - brakeRelease;    //    run brake down to zero
  }
  slip();   // check for clumsy regulator use      
  if (slipping == LOW) {             //  skip if slipping  (HIGH)
    speedUp = regulator * gear;
    speedUp = sqrt(speedUp);
    speedDown = brake;
  }
  acceleration = speedUp - speedDown;  
  acceleration = (acceleration / train);
  acceleration =(acceleration / 5);   //  smaller steps
}  //  ends calculate

void motorDrive() {
  if (esLatch == HIGH ) { // if no probs
    motorValue = motorValue + acceleration;
    if (motorValue < 0) {
      motorValue = 0;
    }
    if (motorValue > 255) {
      motorValue = 255;
    }
    analogWrite(9, motorValue) ;    //  to train
  }
}

void EstopSet() {
  esButton = digitalRead(3);        //   button  pressed ?
  if (esButton == LOW) {
          delay(1000);                  //   slow press
      esButton = digitalRead(3);    //   read ES button
      if (esButton == HIGH) {       //   released again
    esLatch = LOW;
    TRIP = HIGH;
    digitalWrite(2, HIGH);          //  ESTOP RED LED ON
    analogWrite(9, 0);              //   kill output
    motorValue = 0;                 //   0 before things back online
    regulator = 0;
  }
}
}
void EstopReset() {                 //   FLASH LED
  if (esLatch == LOW) {             //   if not - skip TO END
    
    esButton = digitalRead(3);      //   read ES button
    if (esButton == LOW) {          //   LOOKING FOR pressed
        digitalWrite(2, LOW);     //   ESTOP RED LED OFF
        delay(1000);                  //   slow press
        digitalWrite(2, HIGH);    //  ESTOP RED LED ON
        esButton = digitalRead(3);    //   read ES button
        
          if (esButton == HIGH) {       //   if RELEASED
          esLatch = HIGH;             //   reset
          TRIP = LOW;
          digitalWrite(2, LOW);       //  ES RED LED OFF
      } else {
        digitalWrite(2, HIGH);       //  ES RED LED ON - ALARM STILL ON
        delay(1000);
      }
    }
  }
}

void slip() {
  //  if   Heavy train,  just starting,  gear wide & too much regulator
  if ((train > 10) && (motorValue < 30) && (gear > 150) && (regulator > 100)) {
    slipping = HIGH;
    Serial.print(" !! Wheel slip ");
    digitalWrite (11, HIGH);         //   WHEEL SLIP LED
    speedUp = 0;
    speedDown = 50;
  }  else  {
    digitalWrite (11, LOW);          //  LED OFF
    slipping = LOW;
  }
}
