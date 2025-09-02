#include <WiFi.h>
#include <HTTPClient.h>

// WiFi credentials
const char* ssid     = "ipone";
const char* password = "1234567890";

// Server endpoint
const char* serverName = "http://your-server.com/joystick.php";  

// Analog pins
const int xPin = 34;  
const int yPin = 35;  

// Thresholds
const int centerThreshold = 200;  
const int analogMid = 2048;       

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
}

void loop() {
  String direction = "CENTER";

  // ---- Step 1: Wait for CENTER ----
  while (true) {
    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);

    if (isCenter(xValue, yValue)) {
      Serial.println("Joystick is in CENTER");
      break; // Exit loop when centered
    }
    delay(100);
  }

  // ---- Step 2: Wait for a direction ----
  while (true) {
    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);

    direction = getDirection(xValue, yValue);

    if (direction != "CENTER") {
      Serial.println("Sending Direction: " + direction);
      sendDirection(direction);
      break; // Exit after sending one direction
    }
    delay(100);
  }

  // ---- Step 3: Wait until joystick returns to CENTER ----
  while (true) {
    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);

    if (isCenter(xValue, yValue)) {
      Serial.println("Returned to CENTER, ready for next move");
      break; // Restart main loop
    }
    delay(100);
  }
}

// ---- Helper: determine if centered ----
bool isCenter(int x, int y) {
  return (abs(x - analogMid) < centerThreshold &&
          abs(y - analogMid) < centerThreshold);
}

// ---- Helper: determine direction ----
String getDirection(int x, int y) {
  if (x > analogMid + centerThreshold) return "RIGHT";
  if (x < analogMid - centerThreshold) return "LEFT";
  if (y > analogMid + centerThreshold) return "FRONT";
  if (y < analogMid - centerThreshold) return "BACK";
  return "CENTER";
}

// ---- Send data to server ----
void sendDirection(String dir) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "direction=" + dir;

    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
