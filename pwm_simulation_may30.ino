//  PWM train controller with multi simulation of driving a steam locomotive
//  option  SIMPLE, regulator only,  
//  COMPLICATED  with regulator, gear, brake and train weight
//  Estop button pin 2-  overcurrent pin 13 to trip for > 0.7A  -  (hardware - 1R  or 1.4A with 0.5R)

//  08/05/21     compiles   reading inputs works (1 pot and fixed variables)
//  09/05/21     latching alarms worked out   compiles
//  16/05/21    Brake release  -  OFF not bottom of track
//              TRIP reset built into LOOP - for if system tripped
//   21/05/21   "simple"  switch  read in first loop on startup
//   24/05/21   Wheel Slip  (LED pin7)  compiles . (hardware switch for +5 or USB)
//              set to start with Estop set ( TRIP = HIGH)
//   28/05/21   TESTS OK for Pots and modes and LEDs    :-)
//              OC and ES procs done - compiles
//   30/05/21   Working on Brakes COMPILES
//   31/05/21   alarm sets and resets in main loop    COMPILES

//   5V PWM output on pin 9   give 12V PWM via H Bridge to the meters and track  WORKS.
//   Hbridge control hardwired one direction
//   maximised PWM Speed for pin 9

int motorValue = 0;    //  final output pwm value
int regulator = 0;
int brakePrev = 0;     //  only act on increase
int brake = 0;         //  to calculate
int brakePotNow = 0;   //  read pot
int brakePotPrev = 0;  //  last time around
int brakeRelease = 150;  //  release rate  to taste  ****
int train = 0;         //  weight sim
int gear = 0;          //  zero midway
int speedUp = 0;
int speedDown = 0;
int acceleration = 0;
bool Estop = HIGH;
bool esLatch = HIGH;
bool overCurrent = HIGH;
bool ocLatch = HIGH;
bool esButton = HIGH;
bool TRIP = HIGH;      //  HIGH means TRIP has occurred
//   safety measure    //  so Estop must be reset before starting
bool Simple = LOW;     //  Read switch
bool Modeinit = LOW;       //  check simple once only

void setup() {
  
  pinMode(2, OUTPUT);   //   LED for Estop
  pinMode(3, INPUT);    //   OC or Estop reset button
  pinMode(4, OUTPUT);   //   Simple LED
  pinMode(5, INPUT);    //   SIMPLE switch
  pinMode(6, OUTPUT);   //   Mid Gear LED
  pinMode(7, OUTPUT);   //   REVERSE relay
  pinMode(8,OUTPUT);    //   Wheelslip LED 
  pinMode(9, OUTPUT);   //   Train PWM 
  pinMode(10, OUTPUT);  //   brake force LED pwm 
                        //   free
  pinMode(12, OUTPUT);  //   Overcurrent LED
  pinMode(13, INPUT);   //   Overcurrent read

  //  A0 = train pot
  //  A2 = Brake pot
  //  A4 = regulator pot
  //  A5 = Gear pot

  Serial.begin(9600);

  // set PWM freq for pin 9  
  TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 divisor to     1 for PWM frequency of 31372.55 Hz
}

void loop(){

  if(Modeinit == LOW){    //   check mode switch at startup only
    Simple = digitalRead(5);
    digitalWrite(4,Simple);           //  simple led
    Modeinit = HIGH;    
  }

//   start of loop from second time onwards
  if(TRIP == HIGH){         //  see if it's ok to reset now
    EstopReset();
    ocReset();     
  }

  if(Simple == HIGH){       //  set on startup (above)
    simple();
  }
  else{
    complicated();
  }
    ocSet();      //    check and set if necessary
    EstopSet();   //    ditto
    
    delay(1000);    //   normal loop speed for diagnostics only
 
}    //  LOOP AROUND



void simple(){
    regulator = (analogRead(A4));
//          Serial.print("Regulator");   
    regulator = (regulator / 5);    //   max < 255
    if ((esLatch == HIGH )&&(ocLatch == HIGH)) {   //  no probs 
        analogWrite(9, regulator) ;
     }
}                          //  ends simple


void complicated(){
   digitalWrite(4,LOW);    //   not simple
   pots();
   calculate();
   motorDrive(); 
//    ocSet();         //   now in main loop 
       
}    //  endof Complicated

void pots() {      //   read values
  
  regulator = (analogRead(A4));    
  regulator = (regulator / 5);
//        Serial.print("regulator ");
//        Serial.print(regulator);

//  Brakes
//  MODIFied so brakes do not follow pot down but stay on until released
   brakePotNow  = (analogRead(A2));
 if ( brakePotNow < brakePotPrev) {  //  pushing down
      brake = brakePotPrev;          //  keep prev value
   }   
  if(brakePotNow < 30) {    //     knob pulled down to "Release"
     brake = brake - brakeRelease;
        brake = (brake / 4);         //0 to 255 approx -> calculations
  }  
   if(brake < 0){
      brake = 0;
   }
    analogWrite (10 , brake);   //   proportional LED
  if( motorValue > 80) {
    brake = 20;        //  residual - simulate friction
  }
       Serial.print(" brake ");
       Serial.print(brake);
           
  train = (analogRead(A0));     weight of train
  train = (train / 50);
      if(train < 10) {
    train = 10;      //   light engine
  }
       Serial.print(" train ");
       Serial.print(train);
       Serial.print("  ");

  gear = (analogRead(A5));    //  works well
  if (gear > 512) {
    digitalWrite(7, LOW);  //   Reverse relay off
    gear = (gear - 512) / 2;   //   255 max
  }
  else   {
  if (gear < 512) {
     digitalWrite( 7, HIGH);  //   Reverse relay on
 //           Serial.print(" REVERSE ");
     gear = (512 - gear) / 2;  //  255 max
  }
  }
  if (gear < 20){
    digitalWrite(6 ,HIGH);   //  dead zone  LED
    gear = 0;       //  dead zone / mid gear
  }
  else {
     digitalWrite(6 ,LOW);
  }    
 //      Serial.print(" gear ");
 //      Serial.print(gear);
 //      Serial.print(" "); 
}    //   end pots

void calculate() {
  //   if standing - release brakes
  if((motorValue == 0) && (brake > 20)) {
    brake = brake - brakeRelease;    //    run brake down to zero
}
   slip();        //    check for clumsy regulator use      //  move down a bit  ***
   
   speedUp = regulator * gear;
   speedUp = sqrt(speedUp);
 //       Serial.print(" speedUp ");
 //       Serial.print(speedUp); 

   speedDown = brake;   
 //        Serial.print(" speedDown ");
  //       Serial.print(speedDown); 
    
   acceleration = speedUp - speedDown;  // pos or neg
 //        Serial.print(" accel ");
 //        Serial.print(acceleration);  
   acceleration = (acceleration / train);
 //        Serial.print(" accel2 ");
 //        Serial.print(acceleration);
 //        Serial.print("");
}

void motorDrive() {
  if ((esLatch == HIGH )&&(ocLatch == HIGH)) {   //  no probs 
      motorValue = motorValue + acceleration;      
  //   limit value range
      if(motorValue < 0) {
        motorValue = 0;
  }
      if(motorValue > 255) {
         motorValue = 255;
  }
           Serial.print(" motorValue ");
           Serial.println(motorValue);
     analogWrite(9, motorValue) ;    //  to train
}
}

void EstopSet(){
    Estop = digitalRead(3);   //   button  pressed ?
  if (Estop == LOW) {
    esLatch = LOW;
    TRIP = HIGH;
    digitalWrite(2,HIGH);   //  ESTOP RED LED ON
    analogWrite(9,0);      //   kill output
    motorValue = 0;        //    0 before things back online 
  }
}

void EstopReset() {
  if(esLatch == LOW){
     esButton = digitalRead(3);   //   ES button   
  }
     if (esButton == LOW){
        esLatch = HIGH;
        TRIP = LOW;
        digitalWrite(2,LOW);   //  ES RED LED OFF
  }
 }

 void ocSet(){
   overCurrent = digitalRead(13);
  if (overCurrent == LOW) {
    ocLatch = LOW;
    TRIP = HIGH;
    digitalWrite(12,HIGH);  //  OVERCURRENT RED LED 
    analogWrite(9,0);      //   kill output
    motorValue = 0;        //   0 before things back online
  }
 }

void ocReset() {
  if(ocLatch == LOW){
     esButton = digitalRead(3);   //   Estop button to reset  
  }
     if (esButton == LOW){
        ocLatch = HIGH;
        TRIP = LOW;
        digitalWrite(12,LOW);   //  OC RED LED OFF
  }
 }

 void slip(){
  if ((train >100) && (motorValue < 30)){        //    Heavy train, just starting
    if((gear > 150) && (regulator >150)) {      //    gear wide & too much regulator
      digitalWrite (8,HIGH);                    //   WHEEL SLIP LED
      speedUp = 0;
      speedDown = 50;   
    }
  }  else  {
    digitalWrite (8,LOW);                      //  LED OFF
  }
 }
