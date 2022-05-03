
//  PWM paraboliconly  from  PWM train sim final.ino

//  CaderIfor by Michael Beer

//  03/05/22  motorValue preserved for next loop by having motorOut as final tweaked value
//  WORKS   :-)    Slip appears not working

//  02/05/22
//  motorValue = 15.9 * (sqrt(motorValue)); inserted in motorDrive
//  and  regulator = 15.9*(sqrt(regulator));    into Simple
//  plus print  value output to track as comma delimited

//  from  PWM train controller with multi simulation of driving a steam locomotive
//  modes  SIMPLE or   COMPLICATED - with regulator, gear, brake and train weight
//  24/06/21  WORKING   :-)
//  refer back for history

//   5V PWM output on pin 9   give 12V PWM via H Bridge to the meters and track  WORKS.
//   Hbridge control hardwired one direction

float regulator = 0;     //  reg * gear overflows int variable
float gear = 0;
float speedUp = 0;
int velocity = 0;
int motorValue = 0;      //  final output pwm value
int motorOut = 0;        //  final final bug fix attempt
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

  //  A0 = train pot
  //  A2 = Brake pot
  //  A4 = regulator pot
  //  A5 = Gear pot

  // set PWM freq for pin 9
  TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 divisor to     1 for PWM frequency of 31372.55 Hz
  Serial.begin(9600);
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

  delay(100);    //   set loop speed
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
     velocity = (analogRead(A4));
  velocity = (velocity / 4.02);    //   max < 255
  if (esLatch == HIGH ) { //  no probs
    velocity = 15.9 * (sqrt(velocity));
     if(velocity > 0){
         Serial.print(",");      
         Serial.print(velocity);
      }
    analogWrite(9, velocity) ;
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
  brakePotPrev = brake;               //     for next time
  
  brakePotNow  = (analogRead(A2));
  if (brakePotNow < 100) {            //  knob pulled down to "Release"
      brakePotPrev = brakePotNow;     //  avoid popup ??
    brakeRelease = (100 - brakePotNow);
  }  else  {
    brakeRelease = 0;
  }
  brake = brake - brakeRelease;

  if (brake < 0) {
    brake = 0;
  } 
  
  //  thus far 0 to 1024
  brake = (brake / 4); //0 to 255 approx -> calculations
  
  if (( motorValue > 80) && (brake < 1)) { //  bowling along
    brake = 10;                       //  simulate friction
  }
  if (brake > 30) {
    analogWrite (10 , brake);         //   proportional LED
  } else {
    analogWrite (10 , 0); 
  }
  
  train = (analogRead(A0));           // weight of train
  train = (train / 45);
  if (train < 5) {
    train = 5;                       //   light engine
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
      motorOut = 15.9 * (sqrt(motorValue)); //  slope tweak  
       if(motorValue > 0){
         Serial.print(",");
         Serial.print(motorOut);
      }
      analogWrite(9, motorOut) ;    //  to train
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
