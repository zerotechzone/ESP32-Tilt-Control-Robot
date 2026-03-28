#include <esp_now.h>
#include <WiFi.h>

// Motor pins
#define IN1 6
#define IN2 7
#define IN3 8
#define IN4 9

// Enable pins
#define ENA 5
#define ENB 10

// PWM channels
#define CH_A 0
#define CH_B 1

int speedA = 200;
int speedB = 200;

typedef struct {
  char command[10];
} Message;

Message msg;

// PWM setup
void setupPWM() {
  ledcSetup(CH_A, 5000, 8);
  ledcSetup(CH_B, 5000, 8);

  ledcAttachPin(ENA, CH_A);
  ledcAttachPin(ENB, CH_B);
}

// Movement functions
void moveForward() {
  ledcWrite(CH_A, speedA);
  ledcWrite(CH_B, speedB);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBack() {
  ledcWrite(CH_A, speedA);
  ledcWrite(CH_B, speedB);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void moveLeft() {
  ledcWrite(CH_A, speedA);
  ledcWrite(CH_B, speedB);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveRight() {
  ledcWrite(CH_A, speedA);
  ledcWrite(CH_B, speedB);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors() {
  ledcWrite(CH_A, 0);
  ledcWrite(CH_B, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Receive callback
void onReceive(const uint8_t *mac, const uint8_t *data, int len) {
  memcpy(&msg, data, sizeof(msg));

  Serial.print("Received: ");
  Serial.println(msg.command);

  // YOUR REMAPPING
  if (strcmp(msg.command, "FORWARD") == 0) moveLeft();
  else if (strcmp(msg.command, "BACK") == 0) moveRight();
  else if (strcmp(msg.command, "RIGHT") == 0) moveForward();
  else if (strcmp(msg.command, "LEFT") == 0) moveBack();
  else stopMotors();
}

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  setupPWM();

  WiFi.mode(WIFI_STA);


  Serial.print("Receiver MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);
}

void loop() {
}