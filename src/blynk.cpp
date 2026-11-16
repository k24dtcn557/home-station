#include "app.h"

#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp32.h>

void init_blynk() {
  if (strcmp(BLYNK_AUTH_TOKEN, "YourAuthTokenHere") == 0 || strlen(BLYNK_AUTH_TOKEN) == 0) {
    Serial.println("[Blynk] Auth token is not configured. Skipping Blynk setup.");
    return;
  }

  Serial.println("[Blynk] Initializing Blynk...");
  Blynk.config(BLYNK_AUTH_TOKEN);
}

void run_blynk() {
  if (strcmp(BLYNK_AUTH_TOKEN, "YourAuthTokenHere") == 0 || strlen(BLYNK_AUTH_TOKEN) == 0) {
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Blynk.run();
  }
}

void blynk_send_climate(float temperature, float humidity) {
  if (strcmp(BLYNK_AUTH_TOKEN, "YourAuthTokenHere") == 0 || strlen(BLYNK_AUTH_TOKEN) == 0) {
    return;
  }

  if (Blynk.connected()) {
    float temp_to_send = use_fahrenheit ? (temperature * 1.8f + 32.0f) : temperature;
    Blynk.virtualWrite(BLYNK_PIN_TEMP, temp_to_send);
    Blynk.virtualWrite(BLYNK_PIN_HUM, humidity);
    Serial.printf("[Blynk] Sent climate - Temp: %.1f, Hum: %.1f%%\n", temp_to_send, humidity);
  }
}
