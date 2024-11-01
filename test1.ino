int run;

void setup() {
  // put your setup code here, to run once:
  pinMode(7, INPUT_PULLUP);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  run = 0;
}

void loop() {

  if (digitalRead(7) == LOW) {
    while (digitalRead(7) == LOW)
    {
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
    }
    if (run == 1) {
      run = 0;
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
    } else {
      run = 1;
    }
  }
    

  if (run) {
   digitalWrite(8, HIGH);
   digitalWrite(9, LOW);
   delay(150);
   digitalWrite(8, LOW);
   digitalWrite(9, HIGH);
   delay(150);
  }

}
