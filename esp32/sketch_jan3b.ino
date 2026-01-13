#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#define BTN_PIN 12

const char* WIFI_NAME = "TrueWIFI@T_HOME_2.4G";
const char* WIFI_PASS = "1212312121";

String serverUrl = "https://malcolm-biliteral-leanne.ngrok-free.dev/alert";

WiFiClientSecure client;

bool lastBtn = HIGH;
unsigned long lastSend = 0;

void sendLine() {
  client.setInsecure();
  HTTPClient http;
  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "application/json");

  String body = "{\"message\":\"🖊 มีการซื้อปากกาเกิดขึ้น\"}";
  http.POST(body);
  http.end();

  Serial.println("LINE SENT");
}

void setup() {
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT_PULLUP);

  WiFi.begin(WIFI_NAME, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  Serial.println("ESP32 READY");
}

void loop() {
  bool btn = digitalRead(BTN_PIN);

  // กดครั้งเดียว
  if (lastBtn == HIGH && btn == LOW) {
    // cooldown 5 วิ
    if (millis() - lastSend > 5000) {
      sendLine();
      lastSend = millis();
    }
  }

  lastBtn = btn;
}
