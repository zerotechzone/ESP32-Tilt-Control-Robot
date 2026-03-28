#include <Wire.h>
#include <Adafruit_ADXL345_U.h>
#include <esp_now.h>
#include <WiFi.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();

// Receiver MAC
uint8_t receiverMAC[] = {0x48, 0xF6, 0xEE, 0x22, 0x13, 0xBC};

typedef struct {
  char command[10];
} Message;

Message msg;

void setup() {
  Serial.begin(115200);

  Wire.begin(4, 5); // SDA, SCL

  if (!accel.begin()) {
    Serial.println("ADXL345 not found");
    while (1);
  }

  WiFi.mode(WIFI_STA);


  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  // Add receiver
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Peer add failed");
  }
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  float x = event.acceleration.x;
  float y = event.acceleration.y;

  if (y > 5) strcpy(msg.command, "FORWARD");
  else if (y < -5) strcpy(msg.command, "BACK");
  else if (x > 5) strcpy(msg.command, "RIGHT");
  else if (x < -5) strcpy(msg.command, "LEFT");
  else strcpy(msg.command, "STOP");

  esp_now_send(receiverMAC, (uint8_t *)&msg, sizeof(msg));

  Serial.print("Sent: ");
  Serial.println(msg.command);

  delay(200);
}