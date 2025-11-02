/*
        LED ARRANGEMENT
       -----------------
      |       (1)       |
       -----------------
   ___                     ___
  |   |                   |   |
  |   |                   |   |
  |(2)|        (5)        |(4)| 
  |   |                   |   | 
  |___|                   |___|
      ------------------
      |       (3)       |
      ------------------ 
*/

// Define pins for led region 1
#define ON_OFF_LED_1 22
#define DOWN_LED_1 23
#define MODE_LED_1 24
#define UP_LED_1 25

// Define pins for led region 2
#define ON_OFF_LED_2 26
#define DOWN_LED_2 27
#define MODE_LED_2 28
#define UP_LED_2 29

// Define pins for led region 3
#define ON_OFF_LED_3 30
#define DOWN_LED_3 31
#define MODE_LED_3 32
#define UP_LED_3 33

// Define pins for led region 4
#define ON_OFF_LED_4 34
#define DOWN_LED_4 35
#define MODE_LED_4 36
#define UP_LED_4 37

// Define pins for led region 5
#define ON_OFF_LED_5 38

// Stepper motor
#define PUL_PIN 41
#define DIR_PIN 39
#define PUL_PER_MM 50 

int toggle_pins[5][4] = {
  {ON_OFF_LED_1, DOWN_LED_1, MODE_LED_1, UP_LED_1},
  {ON_OFF_LED_2, DOWN_LED_2, MODE_LED_2, UP_LED_2},
  {ON_OFF_LED_3, DOWN_LED_3, MODE_LED_3, UP_LED_3},
  {ON_OFF_LED_4, DOWN_LED_4, MODE_LED_4, UP_LED_4},
  {ON_OFF_LED_5, -1, -1, -1}
};

static int current_mm = 0;
static char buffer[128];  

void toggle_btn(int pin_num) {
  digitalWrite(pin_num, HIGH);
  delay(100);
  digitalWrite(pin_num, LOW);
}

void move_motor_to_position(int target_mm) {
  if ((target_mm > 255) || (target_mm < -5)) {
    Serial.println("ERR: target out of range");
    return;
  }

  int delta = target_mm - current_mm;

  if (delta < 0)
    digitalWrite(DIR_PIN, HIGH);
  else if (delta > 0)
    digitalWrite(DIR_PIN, LOW);
  else {
    Serial.println("OK");  // Không cần di chuyển
    return;
  }

  for (int i = 0; i < abs(delta) * PUL_PER_MM; i++) {
    digitalWrite(PUL_PIN, HIGH); 
    delayMicroseconds(500);
    digitalWrite(PUL_PIN, LOW); 
    delayMicroseconds(500);
  }

  current_mm = target_mm;
  Serial.println("OK");
}

void setup() {
  Serial.begin(115200);

  // setup for LED region
  for (int r = 0; r < 5; r++) {
    for (int c = 0; c < 4; c++) {  
      int pin = toggle_pins[r][c];
      if (pin == -1) break;
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
    }
  }

  // setup for stepper motor
  pinMode(PUL_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
}

// MOTOR HANDLER
void handler_motor_cmd() {
  char *token = strtok(NULL, " ");
  if (token == NULL) {
    Serial.println("ERR: missing arg");
  }
  int target_mm = atoi(token);
  move_motor_to_position(target_mm);
}

// LED HANDLER
void handler_led_cmd() {
  int count = 0;
  for (int r = 0; r < 5; r++) {
    for (int c = 0; c < 4; c++) {
      int pin = toggle_pins[r][c];
      if (pin == -1) break;

      char *token = strtok(NULL, " ");
      if (token == NULL) {
        Serial.println("ERR: missing LED values");
        return;
      }

      count++;
      if (atoi(token) != 0) toggle_btn(pin);
    }
  }
  Serial.println("OK");
}

// HANDLER CMD
void handler_cmd(char* cmd) {
  char *token = strtok(cmd, " ");
  if (token == NULL) {
    Serial.println("ERR: empty command");
    return;
  }

  if (strcmp(token, "motor") == 0) {
    handler_motor_cmd();
  } 
  else if (strcmp(token, "led") == 0) {
    handler_led_cmd();
  } 
  else {
    Serial.println("ERR: unknown command");
  }
}

void loop() {
  if (Serial.available()) {
    int len = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
    buffer[len] = '\0';
    if (len > 0) {
      handler_cmd(buffer);
    }
  }
}
