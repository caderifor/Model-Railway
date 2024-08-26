//   Stroboscope with RPM readout to Serial Monitor
//   MJB   26/08/24
//   up to 24000 rpm  = 400 rps  =  .0025 s/cycle  =  2.5mS  =  2500uS2  
//   period / RPM from one pot
//   pot wired -  CCW value for 960RPM  CW for 24000RPM
//   onOff  in milliseconds  to LED
//   26/08   Flash or print  WORKING  :-)


unsigned long pot = 0;     //  sets period
unsigned long period = 0;
unsigned long onOff = 0;
unsigned long cycleDur = 0;
unsigned long cyclepersec = 0;
unsigned long RPM = 0;
int Readout = 0;       //   for freeze and print RPM


void setup() {
  pinMode(9, OUTPUT);
  pinMode(5, INPUT);
}


void loop() {

  pot = analogRead(1);   //   0 - 1023
  pot = (pot + 1 );   //   1 to 1024
  //  want 2500 uS to 60000uS
  cycleDur = (pot * 56);  // graph slope
  cycleDur = (cycleDur + 2500);  // displaced from zero
  //  in microseconds
  onOff = (cycleDur / 2);      //  half cycle  microSeconds
  onOff = (onOff / 1000);     //  milliseconds
  cyclepersec = (1000000 / cycleDur); //  full cycle

  RPM = (cyclepersec * 60);   //  For final readout

  //   Flash or Print
  Readout = digitalRead(5);
  if (Readout < 1) {
      Serial.begin(9600);
    printFreq() ;
    Serial.end();
  }
  else {
    flash();
  }

}  //    end loop


void printFreq() {
//  Serial.print(pot);
//  Serial.print("  onOff Seconds = ");
//  Serial.print(onOff);
  Serial.print("  Freq hz = ");
  Serial.print(cyclepersec);
  Serial.print("      RPM = ");
  Serial.println(RPM);
}

void flash() {
  //LED on
  digitalWrite(9, HIGH);
  delay(onOff);
  digitalWrite(9, LOW);

  // LED off
  delay(onOff);
  //   end of flash then loop to next flash
}
