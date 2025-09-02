#include <esp_now.h>
#include <WiFi.h>
#include <Servo.h>

// Servo objects
Servo xServo;
Servo yServo;

// Pins for servos
const int xServoPin = 18;
const int yServoPin = 19;

// Structure to receive data
typedef struct struct_message {
  int xAngle;
  int yAngle;
} struct_message;

struct_message incomingData;

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingDataBytes, int len) {
  memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));
  
  xServo.write(incomingData.xAngle);
  yServo.write(incomingData.yAngle);

  Serial.println("Received: X=" + String(incomingData.xAngle) + " Y=" + String(incomingData.yAngle));
}

void setup() {
  Serial.begin(115200);

  // Init Wi-Fi in station mode
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register receive callback
  esp_now_register_recv_cb(OnDataRecv);

  // Attach servos
  xServo.attach(xServoPin);
  yServo.attach(yServoPin);
}

void loop() {
  // Nothing needed, ESP-NOW handles communication
}
