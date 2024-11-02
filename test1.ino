#define PORT_PWM_DIOD_RED 3
int fill_factor, change;

void setup() {
  Serial.begin(9600);
  pinMode(PORT_PWM_DIOD_RED, OUTPUT);
  fill_factor = 0;
  change = 2;
  Serial.println("[INFO] Starting the PWM Arduino program");
}

void loop() {
  analogWrite(PORT_PWM_DIOD_RED, fill_factor);

  if(fill_factor < 255) {
    fill_factor += change;
  } else {
    fill_factor = 0;
  }

  delay(50);

}
