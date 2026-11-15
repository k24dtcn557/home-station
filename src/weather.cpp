#include "weather.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <math.h>
#include <stdlib.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "config.h"
#include "ui.h"

namespace {

constexpr uint32_t kWifiRetryIntervalMs = 10000;
constexpr uint32_t kWeatherRefreshIntervalMs = 10 * 60 * 1000;
constexpr uint32_t kDht12RefreshIntervalMs = 5000;
constexpr uint32_t kDht12ResponseDelayMs = 50;
constexpr size_t kMaxIconBytes = 8 * 1024;
constexpr uint8_t kDht12Address = 0x5C;
constexpr uint8_t kDht12SdaPin = 27;
constexpr uint8_t kDht12SclPin = 22;

uint32_t next_wifi_attempt = 0;
uint32_t next_weather_refresh = 0;
uint32_t next_dht12_refresh = 0;

bool read_dht12(float* temperature_c, float* humidity_percent) {
  Wire.beginTransmission(kDht12Address);
  Wire.write(0);
  const uint8_t transmission_status = Wire.endTransmission();
  if (transmission_status != 0) {
    Serial.printf(
        "[DHT12] I2C transaction failed: %u\n",
        transmission_status);
    return false;
  }

  delay(kDht12ResponseDelayMs);
  const uint8_t bytes_received =
      Wire.requestFrom(kDht12Address, static_cast<uint8_t>(5));
  if (bytes_received != 5) {
    Serial.printf(
        "[DHT12] Expected 5 bytes, received %u\n",
        bytes_received);
    return false;
  }

  uint8_t data[5];
  for (uint8_t index = 0; index < 5; ++index) {
    if (!Wire.available()) {
      Serial.println("[DHT12] Data read interrupted");
      return false;
    }
    data[index] = Wire.read();
  }

  if (static_cast<uint8_t>(data[0] + data[1] + data[2] + data[3]) !=
      data[4]) {
    Serial.println("[DHT12] Checksum failed");
    return false;
  }

  const bool negative_temperature = (data[2] & 0x80) != 0;
  const float temperature =
      static_cast<float>(data[2] & 0x7F) + data[3] / 10.0F;
  const float humidity = static_cast<float>(data[0]) + data[1] / 10.0F;
  if (humidity > 100.0F || temperature > 80.0F) {
    Serial.println("[DHT12] Reading out of range");
    return false;
  }

  *temperature_c = negative_temperature ? -temperature : temperature;
  *humidity_percent = humidity;
  return true;
}

void update_dht12() {
  if (millis() < next_dht12_refresh) {
    return;
  }
  next_dht12_refresh = millis() + kDht12RefreshIntervalMs;

  float temperature_c = 0.0F;
  float humidity_percent = 0.0F;
  if (!read_dht12(&temperature_c, &humidity_percent)) {
    Serial.println("[DHT12] Read failed");
    ui_set_sensor_status("DHT12 unavailable");
    return;
  }

  Serial.printf(
      "[DHT12] %.1f C, %.1f %%\n",
      temperature_c,
      humidity_percent);
  ui_set_sensor(temperature_c, humidity_percent);
}

bool read_icon(WiFiClient* stream, uint8_t* buffer, size_t buffer_size, size_t* bytes_read) {
  *bytes_read = 0;
  const uint32_t started_at = millis();

  while (millis() - started_at < 5000 &&
         *bytes_read < buffer_size &&
         (stream->connected() || stream->available())) {
    const size_t available = stream->available();
    if (available == 0) {
      delay(1);
      continue;
    }

    const size_t remaining = buffer_size - *bytes_read;
    const size_t chunk_size = available < remaining ? available : remaining;
    const size_t read_count =
        stream->readBytes(buffer + *bytes_read, chunk_size);
    *bytes_read += read_count;

    if (read_count == 0) {
      break;
    }
  }

  return *bytes_read > 0 && *bytes_read < buffer_size;
}

bool fetch_icon(
    const String& icon_url,
    uint8_t* icon_buffer,
    size_t* icon_size) {
  Serial.printf("[Weather] Icon URL: %s\n", icon_url.c_str());
  WiFiClientSecure secure_client;
  secure_client.setInsecure();
  HTTPClient https;
  if (https.begin(secure_client, icon_url)) {
    https.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    https.setConnectTimeout(5000);
    https.setTimeout(5000);

    const int status_code = https.GET();
    Serial.printf(
        "[Weather] HTTPS icon response: %d (%s)\n",
        status_code,
        https.errorToString(status_code).c_str());
    if (status_code == HTTP_CODE_OK) {
      WiFiClient* stream = https.getStreamPtr();
      const bool success =
          read_icon(stream, icon_buffer, kMaxIconBytes, icon_size);
      https.end();
      return success;
    }
    https.end();
  } else {
    Serial.println("[Weather] HTTPS icon request setup failed");
  }

  String http_url = icon_url;
  http_url.replace("https:", "http:");
  WiFiClient plain_client;
  HTTPClient http;
  if (!http.begin(plain_client, http_url)) {
    Serial.println("[Weather] HTTP icon request setup failed");
    return false;
  }
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  http.setConnectTimeout(5000);
  http.setTimeout(5000);

  const int status_code = http.GET();
  Serial.printf(
      "[Weather] HTTP icon response: %d (%s)\n",
      status_code,
      http.errorToString(status_code).c_str());
  if (status_code != HTTP_CODE_OK) {
    http.end();
    return false;
  }

  WiFiClient* stream = http.getStreamPtr();
  const bool success = read_icon(stream, icon_buffer, kMaxIconBytes, icon_size);
  http.end();
  return success;
}

bool fetch_weather() {
  WiFiClientSecure client;
  client.setInsecure();

  String url = "https://api.weatherapi.com/v1/current.json?key=";
  url += WEATHER_API_KEY;
  String location = WEATHER_LOCATION;
  location.replace(" ", "%20");
  url += "&q=";
  url += location;
  url += "&aqi=no";
  Serial.printf(
      "[Weather] GET current weather for %s\n",
      WEATHER_LOCATION);

  HTTPClient http;
  if (!http.begin(client, url)) {
    Serial.println("[Weather] Request setup failed");
    return false;
  }
  
  http.setConnectTimeout(5000);
  http.setTimeout(5000);

  const int status_code = http.GET();
  Serial.printf("[Weather] Weather response: %d\n", status_code);
  if (status_code != HTTP_CODE_OK) {
    http.end();
    return false;
  }

  DynamicJsonDocument document(4096);
  const String response_body = http.getString();
  Serial.println("[Weather] API response:");
  Serial.println(response_body);
  const DeserializationError parse_error =
      deserializeJson(document, response_body);
  http.end();
  if (parse_error) {
    Serial.printf(
        "[Weather] JSON parse failed: %s\n",
        parse_error.c_str());
    return false;
  }

  const JsonVariant temperature_value = document["current"]["temp_c"];
  if (temperature_value.isNull()) {
    Serial.println("[Weather] Response has no valid current.temp_c");
    return false;
  }

  const float temperature = temperature_value.as<float>();
  Serial.printf("[Weather] Parsed temp_c: %.2f C\n", temperature);
  const char* condition =
      document["current"]["condition"]["text"].as<const char*>();
  const char* icon_path =
      document["current"]["condition"]["icon"].as<const char*>();
  if (isnan(temperature) || condition == nullptr || icon_path == nullptr) {
    Serial.println("[Weather] Response missing weather fields");
    return false;
  }

  String icon_url = "https:";
  icon_url += icon_path;
  uint8_t* icon_buffer =
      static_cast<uint8_t*>(malloc(kMaxIconBytes));
  if (icon_buffer == nullptr) {
    ui_set_weather(temperature, condition, nullptr, 0);
    return true;
  }

  size_t icon_size = 0;
  if (!fetch_icon(icon_url, icon_buffer, &icon_size)) {
    Serial.println("[Weather] Icon download failed");
    free(icon_buffer);
    ui_set_weather(temperature, condition, nullptr, 0);
    return true;
  }

  ui_set_weather(temperature, condition, icon_buffer, icon_size);
  Serial.printf(
      "[Weather] Updated: %.1f C, %s\n",
      temperature,
      condition);
  free(icon_buffer);
  return true;
}

}  // namespace

void weather_init() {
  pinMode(kDht12SdaPin, INPUT_PULLUP);
  pinMode(kDht12SclPin, INPUT_PULLUP);
  Wire.begin(kDht12SdaPin, kDht12SclPin);
  Wire.setClock(100000);
  Serial.printf(
      "[DHT12] I2C initialized SDA=%u SCL=%u, levels=%d/%d\n",
      kDht12SdaPin,
      kDht12SclPin,
      digitalRead(kDht12SdaPin),
      digitalRead(kDht12SclPin));
  ui_set_sensor_status("DHT12 waiting...");
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("[Weather] Connecting to Wi-Fi SSID: %s\n", WIFI_SSID);
  next_wifi_attempt = millis() + kWifiRetryIntervalMs;
  next_weather_refresh = millis() + 5000;
  next_dht12_refresh = millis() + 1000;
  ui_set_weather_status("Connecting...");
  ui_set_system_status("Connecting Wi-Fi");
}

void weather_update() {
  update_dht12();

  if (WiFi.status() != WL_CONNECTED) {
    ui_set_system_status("Wi-Fi disconnected");
    if (millis() < next_wifi_attempt) {
      return;
    }

    next_wifi_attempt = millis() + kWifiRetryIntervalMs;
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    ui_set_weather_status("Connecting...");
    ui_set_system_status("Connecting Wi-Fi");
    return;
  }

  if (millis() < next_weather_refresh) {
    return;
  }

  next_weather_refresh = millis() + kWeatherRefreshIntervalMs;
  ui_set_system_status("Updating weather");
  if (!fetch_weather()) {
    ui_set_weather_status("Weather unavailable");
    ui_set_system_status("Weather API error");
    return;
  }
  ui_set_system_status("Weather online");
}
