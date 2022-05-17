
//  PWM backtrack May 17
//  CaderIfor by Michael Beer


//  17/05/22    brake release WORKS    :-)
//  Knee values doubled to 80  and slip() all work   :-)
//  16/05/22  Brake backloaded from train sim final
//  11/05/22  knee implemented and train value in slip altered
//  10/09/22  Knee pot works - not applied to acceleration yet
//  09/05/22  using the delay(train); in LOOP to slow changes
//  07/05/22  Reducing acceleration more above "knee" line 213
//    Knee pot on A1  Knee LED on 8 (On below knee)
//  03/05/22  motorValue preserved for next loop by having motorOut as final tweaked value
//  WORKS   :-)  strange lag on startup due to V meter sticking

//  02/05/22  "motorValue = 15.9 * (sqrt(motorValue));" tweak discarded 


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
int brakeRelease = 0;
int train = 0;           //  weight sim
int speedDown = 0;
int acceleration = 0;
int FwdRev = 0;
int knee = 1;            //  loco willingness pot
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
  pinMode(8, OUTPUT);   //   Knee LED
  pinMode(9, OUTPUT);   //   Train PWM
  pinMode(10, OUTPUT);  //   brake force LED pwm
  pinMode(11, OUTPUT);  //   SLIP LED

  //  A0 = train pot
  //  A1 = knee pot (loco willingness to start)
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
    EstopReset();
  }

  if (Simple == HIGH) {     //  set on startup (above)
    simple();
  }
  else {
    complicated();
  }
  EstopSet();

  delay(train);    //   set loop speed  //  with Train pot
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
    FwdRev = (512 - gear) / 2; //  255 max
    if (FwdRev < 20) {
      digitalWrite(6 , HIGH);     //  dead zone  LED
    }
    else {
      digitalWrite(6 , LOW);
    }
  }
  velocity = (analogRead(A4));  //   0 to 1024
  velocity = (velocity / 4.02);    //   max < 255
  if (esLatch == HIGH ) {          //  no probs
    analogWrite(9, velocity) ;
  }
}        //  END OF SIMPLE


void complicated() {
  digitalWrite(4, LOW);    //   not simple
  pots();
  calculate();
  motorDrive();
}    //  endof Complicated



void pots() {      //   read values

  regulator = (analogRead(A4));
  regulator = (regulator / 4.1);      //  0 to 255

  brakePotNow  = (analogRead(A2));
  if ( brakePotNow > brakePotPrev) {  //  pushing up
    brake = brakePotNow;              //  use new value
  }   else  {
    brake = brakePotPrev;             //  stay same - ignore reduction
  }
  brakePotPrev = brake;               //     for next time

  brakePotNow  = (analogRead(A2));    //  look for brake release
  brakePotNow = brakePotNow / 4;
  Serial.print( "brakepotnow=" );
  Serial.print(brakePotNow);
  if (brakePotNow < 30) {        //  knob pulled down to "Release"
    brakePotPrev = brakePotNow;     //  avoid popup ??
    brakeRelease = (30 - brakePotNow);
  }  else  {
    brakeRelease = 0;
  }
  Serial.print( "brakeRelease= " );
  Serial.print(brakeRelease);
  brake = brake - brakeRelease;

  if (brake < 0) {
    brake = 0;
  }
  //               WORKS

  //  brake thus far, 0 to 1024
  brake = (brake / 4);                //0 to 255 approx -> calculations
  if (( motorValue > 80) && (brake < 1)) { //  bowling along
    brake = 10;                       //  simulate friction
  }
  Serial.print( " brake" );
  Serial.print(brake);
  if (brake > 30) {
    analogWrite (10 , brake);         //   proportional "brake" LED
  } else {
    analogWrite (10 , 0);
  }


  //  Using TRAIN pot to change delay() in Programme loop
  //  originally delay(100)
  train = (analogRead(A0));           // weight of train
  train = (train / 4);               // / gives max 255
  if (train < 80) {             //  was 5
    train = 80;                       //   light engine
    //    This TRAIN goes into delay(train); //  80 - 255
  }


  knee = analogRead(1);
  knee = (knee /5);   //  0 to 204
  if (knee < 80) {
    knee = 80;          //  80 to 204
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
  slip();                            // check for clumsy regulator use
  if (slipping == LOW) {             //  skip if slipping  (HIGH)
    speedUp = regulator * gear;
    speedUp = sqrt(speedUp);
    speedUp = speedUp / 12;
  }
  if (motorValue < knee) {
    digitalWrite(8, HIGH);
      //  speedUp not changed   fast acceleration to motor start
  } else {
    digitalWrite(8, LOW);
    speedUp = speedUp / 5;  //  normal acceleration thereafter
  }
  Serial.print(" speedUp = ");
  Serial.print(speedUp);

  speedDown = brake;
  speedDown = speedDown / 30;   
  Serial.print(" speedDown = ");
  Serial.print(speedDown);
  acceleration = speedUp - speedDown;
  Serial.print(" acceleration = ");
  Serial.print(acceleration);
}  //  ends calculate

void motorDrive() {
  if (esLatch == HIGH ) { // if no probs
    motorValue = motorValue + acceleration;
    if (motorValue < 0) {
      motorValue = 0;      //  set limits
    }
    if (motorValue > 255) {
      motorValue = 255;    //  set limits
    }
  Serial.print(" motorValue = ");
  Serial.println(motorValue);
  analogWrite(9, motorValue) ;    //  to train
  }
}

void EstopSet() {
  esButton = digitalRead(3);        //   button  pressed ?
  if (esButton == LOW) {
    delay(1000);                    //   slow press
    esButton = digitalRead(3);      //   read ES button
    if (esButton == HIGH) {         //   released again
      esLatch = LOW;
      TRIP = HIGH;
      digitalWrite(2, HIGH);        //  ESTOP RED LED ON
      analogWrite(9, 0);            //   kill output
      motorValue = 0;               //   to 0 before things back online
      regulator = 0;
    }
  }
}
void EstopReset() {                 //   FLASH LED
  if (esLatch == LOW) {             //   if not - skip TO END

    esButton = digitalRead(3);      //   read ES button
    if (esButton == LOW) {          //   LOOKING FOR pressed
      digitalWrite(2, LOW);         //   ESTOP RED LED OFF
      delay(1000);                  //   slow press
      digitalWrite(2, HIGH);        //  ESTOP RED LED ON
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
   //  if   Heavy train,  gear wide & too much regulator
  if ((train > 200)  && (gear > 180) && (regulator > 150)) {
    //    removed    && (motorValue < 30)
    slipping = HIGH;
    //        Serial.print(" !! Wheel slip ");
    digitalWrite (11, HIGH);         //   WHEEL SLIP LED
    speedUp = 0;                     //   loose momentum
    speedDown = 50;
  }  else  {
    digitalWrite (11, LOW);          //  LED OFF
    slipping = LOW;
  }
}
