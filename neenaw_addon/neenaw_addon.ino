//   neenaw EOR  for  Nano    mjb          06/07/24

//   NeeNaw generator for Bascule bridge
//   driving 220R in series with speaker
//   2 opto isolators in opposite orientation, driving two input pins
//   using  Nano to detect polarity of flashing lights drives from a motor shield
//   one polarity for Nee and other for Naw
//   No Volts = No sound - simples

//   A two Opto-isolator cct works drives 7 or 8 high, triggering the appropriate tone.


bool Nee = LOW;
bool Naw = LOW;

void setup() {
  // initialize digital pins.
  pinMode(7, INPUT);    //   Nee
  pinMode(8, INPUT);    //   Naw
  pinMode(9, OUTPUT);   //   speaker audio
}

void loop() {

  Nee = digitalRead(7);
  Naw = digitalRead(8);

  if ((Nee == HIGH) && (Naw == LOW)) {
    tone(9, 1200);      //  higher sound
    digitalWrite(4, HIGH);
  }
  else {
    if ((Naw == HIGH) && (Nee == LOW)) {
      tone(9, 1000);       //  lower sound
      digitalWrite(2, LOW);
    }
    else {
      noTone(9);
    }
  }
}  //    end loop
