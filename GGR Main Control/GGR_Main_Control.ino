/*  GGR - Grandad's Garden Railway
    ALL WORKING
    ---------------------------------------------------------------
    Turns out new PWM has top value 499
    now uses  OCR1A = motorValue9; 
     Provision for overcurrent monitoring - from the Drive Power Amplifier circuit
    speeds now = {0, 100, 150, 190, 240, 290, 340, 390, 440, 499};
*/

#include <IRremote.hpp>
#define PWM_TOP 499     

// ------------------------- IR SETUP -------------------------------
unsigned long ircodereceived = 0;

// ------------------------- PWM SETUP ------------------------------
const int pwmPin = 9;        // 32 kHz PWM
void setupPWM32kHz() {
  pinMode(pwmPin, OUTPUT);
  TCCR1A = _BV(COM1A1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
  ICR1  = PWM_TOP;   // ✔ clean
  OCR1A = 0;         // start stopped
}

// ------------------------- CONTROL VARIABLES ----------------------
bool CCW = LOW;
bool Estop = LOW;         // Start without automatic EMGstop
bool EstopRS = LOW;       // IR Reset flag
bool blowWhistle = LOW;   // IR whistle flag

int overcurrent = 0;      //  read in A0
int count = 0;
int motorValue9 = 0;
int speedPrev = 0;
int speedReq = 0;

bool Point11 = LOW;
bool Point12 = LOW;
bool Point21 = LOW;
bool Point22 = LOW;
bool Point31 = LOW;
bool Point32 = LOW;
bool HoldOff = LOW;

int notchIndex = 0;
int Notch[] = {0, 100, 150, 190, 240, 290, 340, 390, 440, 499};

// ------------------------- FUNCTION PROTOTYPES --------------------
void RESET();
void Calc();
void Whistle();
void Read();
void Points();
void Signals();

// =================================================================
// ============================== SETUP =============================
void setup() {
  Serial.begin(9600);
  delay(2000);

  // ---- IR Receiver ----
  IrReceiver.begin(11);

  // ---- PWM ----
  setupPWM32kHz();

  // ---- Pin Modes ----
  pinMode(2, OUTPUT);   // Signal 1
  pinMode(3, OUTPUT);   // Signal 2
  pinMode(4, OUTPUT);   // Whistle
  pinMode(5, OUTPUT);   // Point 1 to Arduino 2
  pinMode(6, OUTPUT);   // Point 2  to Arduino 2
  pinMode(7, OUTPUT);   // E-stop LED
  pinMode(8, OUTPUT);   // Direction relay
  pinMode(9, OUTPUT);   // PWM output
  pinMode(12, OUTPUT);  // Direction relay
  pinMode(10, OUTPUT);  // Point 3 to Arduino 2
  //  pinMode (A0, INPUT)  //   Analog read overcurrent signal

  // ---- Startup safe state ----
  digitalWrite(7, HIGH);   // Estop LED ON
  digitalWrite(8, LOW);    // Relays OFF
  digitalWrite(12, LOW);   // both together =  track cut off
  OCR1A = motorValue9;     // Motor OFF
}

// =================================================================
// =============================== LOOP =============================
void loop() {

  Read();  // Read IR commands

  // Process flags

  if (EstopRS == HIGH) {    //   or Bool  if (EstopRS)
    RESET();
  }
  if (Estop == HIGH ) {    //  by IR or overcurrent
     EMGstop();
  }

  if (HoldOff == LOW) {
    count++;
    if (count > 1000) {
      Calc();
      count = 0;
    }
  }

  if (blowWhistle) {
    Whistle();
    blowWhistle = LOW;
  }

  Points();
  Signals();
}

// ========================= IR READER ==============================
void Read() {

  if (!IrReceiver.decode()) return;
  uint8_t cmd = IrReceiver.decodedIRData.command;
  if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
    IrReceiver.resume();
    return;
  }

  Serial.print("IR Protocol: ");
  Serial.print(IrReceiver.decodedIRData.protocol);
  Serial.print("  Command: 0x");
  Serial.println(cmd, HEX);

  switch (cmd) {

    case 0x3D: Estop = HIGH; EstopRS = LOW;break;  // Emergency stop
    case 0xEF: EstopRS = HIGH; Estop = LOW;break;  // Reset
    
    case 0x87: blowWhistle = HIGH; break;
    case 0x4E: CCW = HIGH; digitalWrite(12, HIGH); digitalWrite(8, LOW); break;
    case 0x4F: CCW = LOW;  digitalWrite(12, LOW);  digitalWrite(8, HIGH); break;
    case 0x70: Point11 = HIGH; Point12 = LOW; break;
    case 0x71: Point12 = HIGH; Point11 = LOW; break;
    case 0x72: Point21 = HIGH; Point22 = LOW; break;
    case 0x73: Point22 = HIGH; Point21 = LOW; break;
    case 0x03: Point31 = HIGH; Point32 = LOW; break;
    case 0x09: Point32 = HIGH; Point31 = LOW; break;
    case 0x19: notchIndex = 0; break;
    case 0x10: notchIndex = 1; break;
    case 0x11: notchIndex = 2; break;
    case 0x12: notchIndex = 3; break;
    case 0x13: notchIndex = 4; break;
    case 0x14: notchIndex = 5; break;
    case 0x15: notchIndex = 6; break;
    case 0x16: notchIndex = 7; break;
    case 0x17: notchIndex = 8; break;
    case 0x18: notchIndex = 9; break;
    //    Up and down arrows
    case 0x4A: notchIndex++; if (notchIndex > 9) notchIndex = 9; break;
    case 0x4B: notchIndex--; if (notchIndex < 0) notchIndex = 0; break;
    default: Serial.print("Unknown key: 0x"); Serial.println(cmd, HEX); break;
  }
  IrReceiver.resume();

    //  Check for overcurrent
  overcurrent = analogRead(A0);//  A-D out= 0 to 1024
  // Serial.print("overcurrent mon   ");
  // Serial.println(overcurrent);      around 400
  // if(overcurrent > 500){    //   fix threshold by test
  //   Estop = HIGH;
  // }
}     //   end Read

// =================================================================
// ==================Call  POINT SERVOS (in Arduino 2)=============
void Points() {
  if (Point11) digitalWrite  (5, HIGH);
  if (Point12) digitalWrite  (5, LOW);
  if (Point21) digitalWrite  (6, HIGH);
  if (Point22) digitalWrite  (6, LOW);
  if (Point31) digitalWrite  (10, HIGH);
  if (Point32) digitalWrite  (10, LOW);
}   //  end Points
// =================================================================
// =========================== SIGNALS ==============================
void Signals() {
  digitalWrite(2, (CCW && Point11) ? HIGH : LOW);
  digitalWrite(3, (!CCW && Point22 && Point31) ? HIGH : LOW);
}   //  end signals

// =================================================================
// ========================== SPEED CALC ============================
void Calc() {

  speedPrev = motorValue9;// take in previous speed
//  take in  Notch[notchIndex]
   if (notchIndex < 3) {   //  go straight to notch value
  speedPrev = Notch [notchIndex];
  Serial.print("    -   speedPrev    ");
  Serial.print(speedPrev);
  speedReq = speedPrev;
  }

   if (notchIndex > 2) {       //   do   tweening as code cycles
     if (Notch [notchIndex] > speedPrev) {
       speedPrev = speedPrev + 1;
     }
     if (Notch [notchIndex] < speedPrev) {
      speedPrev = speedPrev - 1;
     }
     speedReq = speedPrev;
  }   //    tweening done    Might need tweaking

  //  Check range
  if (speedReq < 0) {
    speedReq = 0;
  }
  if (speedReq > PWM_TOP ) {
    speedReq = PWM_TOP;
  }
  //    output speed
  motorValue9 = speedReq;
  Serial.print("motorValue9  ");
  Serial.println(motorValue9);
  OCR1A = motorValue9;
}   //  end calc

// =================================================================
// ============================= WHISTLE ============================
void Whistle() {
  int frequency = 3000;
  unsigned long duration = 100000;
  int period = 1000000 / frequency;
  int pulse = period / 2;
  unsigned long start = micros();
  while ((micros() - start) < duration) {
    digitalWrite(4, HIGH); delayMicroseconds(pulse);
    digitalWrite(4, LOW);  delayMicroseconds(pulse);
  }
}    //  end whistle


void EMGstop() {
  Serial.println("EMG STOP");
  digitalWrite(7, HIGH);
  HoldOff = HIGH;
  digitalWrite(12, LOW);
  digitalWrite(8, LOW);
  notchIndex = 0;
  motorValue9 = 0;
   OCR1A = 0; 
  Estop = LOW;
}

  
void RESET() {
  Serial.println("RESET");
  digitalWrite(12, LOW);
  digitalWrite(8, HIGH);
  digitalWrite(7, LOW);
  HoldOff = LOW;
  EstopRS = LOW;
}
