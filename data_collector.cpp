#include <WiFi.h>
#include <HTTPClient.h>


const char* ssid     = "ipone";
const char* password = "1234567890";

const char* serverName = "http://your-server.com/joystick.php";  

const int xPin = 34;  
const int yPin = 35;  

const int centerThreshold = 200;  
const int analogMid = 2048;       

void setup() {
  Serial.begin(115200);

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

  while (true) {
    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);

    if (isCenter(xValue, yValue)) {
      Serial.println("Joystick is in CENTER");
      break; 
    }
    delay(100);
  }

  while (true) {
    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);

    direction = getDirection(xValue, yValue);

    if (direction != "CENTER") {
      Serial.println("Sending Direction: " + direction);
      sendDirection(direction);
      break; 
    }
    delay(100);
  }

  while (true) {
    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);

    if (isCenter(xValue, yValue)) {
      Serial.println("Returned to CENTER, ready for next move");
      break; 
    }
    delay(100);
  }
}

bool isCenter(int x, int y) {
  return (abs(x - analogMid) < centerThreshold &&
          abs(y - analogMid) < centerThreshold);
}

String getDirection(int x, int y) {
  if (x > analogMid + centerThreshold) return "RIGHT";
  if (x < analogMid - centerThreshold) return "LEFT";
  if (y > analogMid + centerThreshold) return "FRONT";
  if (y < analogMid - centerThreshold) return "BACK";
  return "CENTER";
}

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
