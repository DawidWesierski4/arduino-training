#include <Servo.h>
#include <LiquidCrystal.h>
#define PORT_ANALOG_FOTO_RES A5

#define LOG_LEVEL_DEBUG   0
#define LOG_LEVEL_INFO    1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR   3

#define LOG_DEBUG(x) if (log_level <= LOG_LEVEL_DEBUG) Serial.println("[DEBUG] " + String(x))
#define LOG_INFO(x) if (log_level <= LOG_LEVEL_INFO) Serial.println("[INFO] " + String(x))
#define LOG_WARN(x) if (log_level <= LOG_LEVEL_WARN) Serial.println("[WARN] " + String(x))
#define LOG_ERROR(x) if (log_level <= LOG_LEVEL_ERROR) Serial.println("[ERROR] " + String(x))

#define SPEED_UP_PIN(x, speed) \
  for (int i = (speed); i < 255; i++) { \
      analogWrite((x), i); \
      delay(25); \
    } \
    (speed) = 255;

#define SPEED_DOWN_PIN(x, speed) \
  for (int i = (speed); i >= 0; i--) { \
      analogWrite((x), i); \
      delay(25); \
    } \
    (speed) = 0; \

String recieve_serial;
int log_level;
int displayed_level, speed;
float tolerance;
int general_delay;


void setup() {
  Serial.begin(9600);
  recieve_serial = "";
  speed = 1;
  log_level = 1;
  general_delay = 200;
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);
  
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
}

void loop() {
  digitalWrite(7, LOW); // DC motor left spin on H-bridge
  digitalWrite(8, HIGH);
  SPEED_UP_PIN(6, speed);
  SPEED_DOWN_PIN(6,speed);
  delay(400);


  digitalWrite(8, LOW); // DC motor right spin on H-bridge
  digitalWrite(7, HIGH);
  SPEED_UP_PIN(6, speed);
  SPEED_DOWN_PIN(6,speed);
  delay(400);

}
