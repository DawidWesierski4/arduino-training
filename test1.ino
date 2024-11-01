#define PORT_DIOD_RED 8
int analog_read = 0;
float potential = 0;
int sleep = 0;
int tmp;

void setup() {
  Serial.begin(9600);
  pinMode(PORT_DIOD_RED, OUTPUT);
}

void loop() {
  tmp = analogRead(A5);
  if (analog_read != tmp) {
    analog_read = tmp;
    potential = analog_read * (5./1024.);
    Serial.println("[INFO] ANALOG A5 READ " + String(potential) + "V");
    sleep = analog_read * 10;
    if (sleep > 10000) sleep = 10000;
    if (sleep < 40 ) sleep = 40;
    Serial.println("[INFO] setting sleep to " + String(sleep));
  }

  delay(sleep);
  digitalWrite(PORT_DIOD_RED, HIGH);
  delay(sleep);
  digitalWrite(PORT_DIOD_RED, LOW);

}
