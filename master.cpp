#include <esp_now.h>
#include <WiFi.h>

// Analog pins for joystick
const int xPin = 34;  
const int yPin = 35;  

// Structure to send data
typedef struct struct_message {
  int xAngle;
  int yAngle;
} struct_message;

struct_message myData;

// Slave MAC address (replace with your slave ESP32’s MAC)
uint8_t slaveAddress[] = {0x24, 0x6F, 0x28, 0xAA, 0xBB, 0xCC};

void setup() {
  Serial.begin(115200);

  // Set device as Wi-Fi Station (needed for ESP-NOW)
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, slaveAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  int xValue = analogRead(xPin);  // 0–4095
  int yValue = analogRead(yPin);  // 0–4095

  // Map ADC values to 0–180 degrees
  myData.xAngle = map(xValue, 0, 4095, 0, 180);
  myData.yAngle = map(yValue, 0, 4095, 0, 180);

  // Send message
  esp_err_t result = esp_now_send(slaveAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent: X=" + String(myData.xAngle) + " Y=" + String(myData.yAngle));
  } else {
    Serial.println("Error sending data");
  }

  delay(50); // update rate
}
