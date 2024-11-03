#include <Servo.h>

#define PORT_PWM_SERVO_1 3
struct Servo_pwm {
  Servo servo;
  int index;
  int position;
  int current_position;
  int change;
  bool automatic;
} servo_1;
String recieve_serial;

void setup() {
  Serial.begin(9600);
  pinMode(PORT_PWM_SERVO_1, OUTPUT);
  recieve_serial = "";
  servo_1.position = 0;
  servo_1.index = 1;
  servo_1.change = 6;
  servo_1.automatic = false;
  servo_1.servo.attach(PORT_PWM_SERVO_1);
  Serial.println("[INFO] Starting the PWM servo Arduino program");
}

void parseToken(Servo_pwm &servo, String token) {
  int separatorIndex = token.indexOf('=');

  if (separatorIndex == -1) {
    Serial.println("[ERROR] Invalid token: " + token);
    return;
  }

  String key = token.substring(0, separatorIndex);
  String value = token.substring(separatorIndex + 1);

  if (key == "automatic") {
    servo.automatic = (value.toInt() != 0);
    Serial.println("[INFO] Set servo" + String(servo.index) + " automatic to " + String(servo.automatic));
  } else if (key == "position") {
    servo.position = value.toInt();
    Serial.println("[INFO] Set servo" + String(servo.index) + " position to " + String(servo.position));
  } else if (key == "change") {
    servo.change = value.toInt();
    Serial.println("[INFO] Set servo" + String(servo.index) + " change to " + String(servo.change));
  } else {
    Serial.println("[ERROR] servo" + String(servo.index) + " Unknown parameter: " + key);
  }
}

void parse_servo_steering_string(Servo_pwm &servo, String input) {
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
    parseToken(servo, token);
  }

  // last command not handled by while
  if (input.length() > 0) {
    parseToken(servo, input);
  }
}

void loop() {
  // Control section
  if (Serial.available() > 0) {
    recieve_serial = Serial.readStringUntil('\n');
    Serial.println("[INFO] Recieved \"" + recieve_serial + "\"");
    if (recieve_serial.startsWith("servo_1")) {
      parse_servo_steering_string(servo_1, recieve_serial);
    recieve_serial = "";
    }
  }

  // automatic
  if (servo_1.automatic) {
    if (servo_1.position < 180) {
      servo_1.servo.write(servo_1.position);
      servo_1.current_position = servo_1.position;
    } else {
      servo_1.position = 0;
      Serial.println("[INFO] Automatic cycle start reseting the servo position to 0");
    }

    servo_1.position += servo_1.change;
    delay(250);

  // manual
  } else {
    if (servo_1.position < 180 && servo_1.current_position != servo_1.position) {
      Serial.println("[INFO] Seting the servo position to  " + String(servo_1.position));
      servo_1.servo.write(servo_1.position);
      servo_1.current_position = servo_1.position;
    }
  }
}
