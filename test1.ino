#include <Servo.h>

#define PORT_PWM_SERVO_1     3
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
int reading_analog;

struct Servo_pwm {
  Servo servo;
  int index;
  int position;
  int current_position;
  int change;
  int delay;
  bool automatic;
} servo_1;


void setup() {
  Serial.begin(9600);
  pinMode(PORT_PWM_SERVO_1, OUTPUT);
  recieve_serial = "";
  servo_1.position = 0;
  servo_1.index = 1;
  servo_1.change = 6;
  servo_1.automatic = false;
  servo_1.delay = 250;
  servo_1.servo.attach(PORT_PWM_SERVO_1);
  LOG_INFO("Starting the PWM servo Arduino program");
  log_level = 1;
}

void servo_parse_token(Servo_pwm &servo, String token) {
  int separatorIndex = token.indexOf('=');

  if (separatorIndex == -1) {
    LOG_ERROR("Invalid token: " + token);
    return;
  }

  String key = token.substring(0, separatorIndex);
  String value = token.substring(separatorIndex + 1);

  if (key == "automatic") {
    servo.automatic = (value.toInt() != 0);
    LOG_INFO("Set servo " + String(servo.index) + " automatic to " + String(servo.automatic));
  } else if (key == "position") {
    servo.position = value.toInt();
    LOG_INFO("Set servo " + String(servo.index) + " position to " + String(servo.position));
  } else if (key == "change") {
    servo.change = value.toInt();
    LOG_INFO("Set servo " + String(servo.index) + " change to " + String(servo.change));
  } else if (key == "delay") {
    servo.delay = value.toInt();
    LOG_INFO("Set servo " + String(servo.index) + " delay to " + String(servo.delay));
  } else {
    LOG_ERROR("servo" + String(servo.index) + " Unknown parameter: " + key);
  }
}

void servo_parse_steering_string(Servo_pwm &servo, String input) {
  int index;
  String token;

  // if starts with servo cut the servo_nmb at the begginging
  if (input.startsWith("servo")) {
    index = input.indexOf(' ');
    input = input.substring(index + 1);
  }
  input.trim();

  while ((index = input.indexOf(' ')) != -1) {
    token = input.substring(0, index);
    input = input.substring(index + 1);
    servo_parse_token(servo, token);
  }

  // last command not handled by while
  if (input.length() > 0) {
    servo_parse_token(servo, input);
  }
}

void servo_control_mechanism(Servo_pwm &servo, String steer = "") {
  // Control section

  // global controls
  if (recieve_serial != "") {
    steer = recieve_serial;
  }

  if (steer.startsWith("servo_" + String(servo.index))) {
    servo_parse_steering_string(servo, steer);
  }

  // Automatic mode
  if (servo.automatic) {
    if (servo.change < 0 && servo.position <= 0) {
      servo.position = 180;
    } else if (servo.change > 0 && servo.position >= 180) {
      servo.position = 0;
    } else {
      servo.position += servo.change;
    }
    servo.servo.write(servo.position);
    servo.current_position = servo.position;
    delay(servo.delay);
  
  // Manual mode
  } else {
    if (servo.position < 180 && servo.current_position != servo.position) {
      LOG_INFO("Setting the servo position to " + String(servo.position));
      servo.servo.write(servo.position);
      servo.current_position = servo.position;
    }
  }
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

  reading_analog = analogRead(PORT_ANALOG_FOTO_RES);
  LOG_DEBUG("Reading analog A5" + String(reading_analog));

  reading_analog = map(reading_analog, 0, 1000, 0, 180);
  LOG_DEBUG("Mapped to 180 reading analog = " + String(reading_analog));

  if (abs(reading_analog - servo_1.current_position) > 5) {
    servo_1.position = reading_analog;
    LOG_INFO("Detected light intensity change " + String(servo_1.position / 180. * 100) + "%");
    delay(150);
  }

  servo_control_mechanism(servo_1);
  recieve_serial = "";
}
