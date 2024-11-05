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

String recieve_serial;
int log_level;
int displayed_level, ret;
float tolerance;
int general_delay;

class ServoPwm {
public:
  Servo servo;
  int index;
  int position;
  int current_position;
  int change;
  int delay_time;
  bool automatic;

  ServoPwm(int idx) : index(idx), position(0), current_position(0), change(0), delay_time(0), automatic(false) {}

  void parseToken(String token) {
    int separatorIndex = token.indexOf('=');
    if (separatorIndex == -1) {
      LOG_ERROR("Invalid token: " + token);
      return;
    }

    String key = token.substring(0, separatorIndex);
    String value = token.substring(separatorIndex + 1);

    if (key == "automatic") {
      automatic = (value.toInt() != 0);
      LOG_INFO("Set servo " + String(index) + " automatic to " + String(automatic));
    } else if (key == "position") {
      position = value.toInt();
      LOG_INFO("Set servo " + String(index) + " position to " + String(position));
    } else if (key == "change") {
      change = value.toInt();
      LOG_INFO("Set servo " + String(index) + " change to " + String(change));
    } else if (key == "delay") {

      delay_time = value.toInt();
      LOG_INFO("Set servo " + String(index) + " delay to " + String(delay_time));
    } else {
      LOG_ERROR("Servo " + String(index) + " Unknown parameter: " + key);
    }
  }

  void parseSteeringString(String input) {
    int index;
    String token;

    if (input.startsWith("servo")) {
      index = input.indexOf(' ');
      input = input.substring(index + 1);
    }
    input.trim();

    while ((index = input.indexOf(' ')) != -1) {
      token = input.substring(0, index);
      input = input.substring(index + 1);
      parseToken(token);
    }

    if (input.length() > 0) {
      parseToken(input);
    }
  }

  void controlMechanism(String steer = "") {
    if (steer.length() == 0 && steer.startsWith("servo_" + String(index))) {
      parseSteeringString(steer);
    }

    if (automatic) {
      if (change < 0 && position <= 0) {
        position = 180;
      } else if (change > 0 && position >= 180) {
        position = 0;
      } else {
        position += change;
      }
      servo.write(position);
      current_position = position;
      delay(delay_time);
    } else {
      if (position < 180 && current_position != position) {
        LOG_INFO("Setting the servo position to " + String(position));
        servo.write(position);
        current_position = position;
      }
    }
  }
};

class FotoresistorReader {
public:
  int index;
  int port;
  int maximum_light_level;
  int minimum_light_level;
  int light_level;

  FotoresistorReader(int idx, int port) : index(idx), port(port), maximum_light_level(1000), minimum_light_level(500), light_level(0) {};

  int readBrightness(void) {
    light_level = analogRead(port);

    if (light_level < minimum_light_level) {
        minimum_light_level = light_level;
    } else if (light_level > maximum_light_level) {
      maximum_light_level = light_level;
    }

    int brightness_percentage = map(light_level, maximum_light_level, minimum_light_level, 0, 100);
    return brightness_percentage;
  }
};

FotoresistorReader foto_read(0, A5);
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);


void setup() {
  Serial.begin(9600);
  recieve_serial = "";
  log_level = 1;
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0); //Ustawienie kursora
  lcd.print("LIGHT LEVEL");
  lcd.setCursor(0,1);
  displayed_level = 0;
  tolerance = 1.05; 
  general_delay = 1000;
}

void general_parse_token(String token) {
  int separatorIndex = token.indexOf('=');

  if (separatorIndex == -1) {
    LOG_ERROR("Invalid token: " + token);
    return;
  }

  String key = token.substring(0, separatorIndex);
  String value = token.substring(separatorIndex + 1);

  if (key == "log_level") {
    log_level = (value.toInt() != 0);
    LOG_INFO("Set log_level to " + String(log_level));
  }
}

void general_parse_steering_string(String input) {
  int index;
  String token;

  // if starts with servo cut the servo_nmb at the begginging
  if (input.startsWith("general")) {
    index = input.indexOf(' ');
    input = input.substring(index + 1);
  }
  input.trim();

  while ((index = input.indexOf(' ')) != -1) {
    token = input.substring(0, index);
    input = input.substring(index + 1);
    general_parse_token(token);
  }

  // last command not handled by while
  if (input.length() > 0) {
    general_parse_token(input);
  }
}


void loop() {
  // reading the serial
  if (Serial.available() > 0) {
    recieve_serial = Serial.readStringUntil('\n');
    LOG_INFO("Received \"" + recieve_serial + "\"");
  }
  general_parse_steering_string(recieve_serial);
  ret = foto_read.readBrightness();
  LOG_INFO("ret = " + String(ret) + " ligt_level = " + String(foto_read.light_level) + "  max " + String( foto_read.maximum_light_level));

  lcd.setCursor(0, 1);
  lcd.print(String(ret));
  lcd.print(" %");
  displayed_level = ret;


  
  delay(general_delay);
}
