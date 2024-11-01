#define DIOD_PORT_RED   9
#define DIOD_PORT_GREEN 8
#define BUTTON_PORT     7
bool port_enabled_red;
bool port_enabled_green;
String recieve;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("[INFO] Interface online");

  pinMode(DIOD_PORT_RED, OUTPUT);
  pinMode(DIOD_PORT_GREEN, OUTPUT);
  pinMode(BUTTON_PORT, INPUT_PULLUP);
  port_enabled_red = 0;
  port_enabled_green = 0;
  recieve = "";
}

bool switch_port(bool state, int port_number) {
  if (state) {
    digitalWrite(port_number, LOW);
    return 0;
  } else {
    digitalWrite(port_number, HIGH);
    return 1;
  }
}

void loop() {
  if(Serial.available() > 0) {
    recieve = Serial.readStringUntil('\n');
    Serial.println("[INFO] Recevied \"" + recieve + "\"");

    if (recieve == "RED" || recieve == "R") {
      port_enabled_red = switch_port(port_enabled_red, DIOD_PORT_RED);
      Serial.println("[INFO] Setting red diod to " + String(port_enabled_red));
    } else if (recieve == "GREEN" || recieve == "G") {
      port_enabled_green = switch_port(port_enabled_green, DIOD_PORT_GREEN);
      Serial.println("[INFO] Setting green diod to " +  String(port_enabled_green));
    }
  }
}
