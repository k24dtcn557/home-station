#include "app.h"

#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp32.h>

static unsigned long last_alert_time = 0;
const unsigned long ALERT_COOLDOWN = 1 * 60 * 1000UL; // Send alert every 1 minute

void init_blynk() {
  Serial.println("[Blynk] Initializing Blynk...");
  Blynk.config(BLYNK_AUTH_TOKEN);
}

void run_blynk() {
  if (WiFi.status() == WL_CONNECTED) {
    Blynk.run();
  }
}

void blynk_send_climate(float temperature, float humidity) {
  if (Blynk.connected()) {
    float temp_to_send = use_fahrenheit ? (temperature * 1.8f + 32.0f) : temperature;
    Blynk.virtualWrite(BLYNK_PIN_TEMP, temp_to_send);
    Blynk.virtualWrite(BLYNK_PIN_HUM, humidity);
    Serial.printf("[Blynk] Sent climate - Temp: %.1f, Hum: %.1f%%\n", temp_to_send, humidity);

    if (temperature > 35.0f && (millis() - last_alert_time > ALERT_COOLDOWN)) {
      Blynk.logEvent("high_temp", String("Cảnh báo: Nhiệt độ trong phòng quá cao: ") + temperature + "°C");
      last_alert_time = millis();
      Serial.printf("[Blynk] Sent climate alert - Temp: %.1f\n", temp_to_send);
    }
  }
}

void blynk_send_screen_power(bool on) {
  if (Blynk.connected()) {
    Blynk.virtualWrite(BLYNK_PIN_SCREEN, on ? 1 : 0);
  }
}

void blynk_send_brightness(uint32_t brightness) {
  if (Blynk.connected()) {
    Blynk.virtualWrite(BLYNK_PIN_BRIGHTNESS, brightness);
    Serial.printf("[Blynk] Synced brightness to Blynk: %u\n", brightness);
  }
}

BLYNK_CONNECTED() {
  Serial.println("[Blynk] Connected to Blynk Cloud.");
  Blynk.virtualWrite(BLYNK_PIN_SCREEN, screen_power ? 1 : 0);
  uint32_t brightness = prefs.getUInt("brightness", 128);
  Blynk.virtualWrite(BLYNK_PIN_BRIGHTNESS, brightness);
}

BLYNK_WRITE(BLYNK_PIN_SCREEN) {
  int value = param.asInt();
  Serial.printf("[Blynk] Received screen switch command: %d\n", value);
  set_screen_power(value != 0);
}

BLYNK_WRITE(BLYNK_PIN_BRIGHTNESS) {
  uint32_t brightness = param.asInt();
  Serial.printf("[Blynk] Received brightness command: %u\n", brightness);
  set_screen_brightness(brightness, false);
}
