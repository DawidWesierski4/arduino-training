#define PORT_PWM_DIOD_RED 3
int change;
byte fill_factor;

void setup() {
  Serial.begin(9600);
  pinMode(PORT_PWM_DIOD_RED, OUTPUT);
  fill_factor = 0;
  change = 4;
  Serial.println("[INFO] Starting the PWM Arduino program");
}

void loop() {
  analogWrite(PORT_PWM_DIOD_RED, fill_factor);

  fill_factor += change;
  if (fill_factor > (255 - abs(change)) || fill_factor < abs(change)) {
    change *= -1;
    Serial.println("[INFO] change switch polarization change = " + String(change));
  }
  delay(40);

}
