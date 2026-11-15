#include "app.h"

namespace {

bool read_dht12(float& temperature, float& humidity) {
  Wire.beginTransmission(DHT12_ADDRESS);
  Wire.write(0);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  if (Wire.requestFrom(DHT12_ADDRESS, 5u) != 5u) {
    return false;
  }

  uint8_t data[5];
  for (uint8_t index = 0; index < 5; ++index) {
    data[index] = Wire.read();
  }

  uint8_t checksum = data[0] + data[1] + data[2] + data[3];
  if (checksum != data[4]) {
    return false;
  }

  humidity = data[0] + data[1] / 10.0f;
  temperature = (data[2] & 0x80) != 0
                    ? -(data[2] & 0x7f) - data[3] / 10.0f
                    : data[2] + data[3] / 10.0f;
  return true;
}

void set_climate_label(lv_obj_t* label, const char* text, const lv_font_t* font,
                       lv_color_t color) {
  lv_label_set_text(label, text);
  lv_obj_set_style_text_font(label, font, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(label, color, LV_PART_MAIN | LV_STATE_DEFAULT);
}

}  // namespace

void create_climate_ui() {
  lv_obj_t* title = lv_label_create(home_page);
  set_climate_label(title, "Indoor climate", get_font_14(), lv_color_hex(0xe4ffff));
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 20, 12);

  climate_temperature = lv_label_create(home_page);
  set_climate_label(climate_temperature, "--.-°", get_font_42(), lv_color_white());
  lv_obj_align(climate_temperature, LV_ALIGN_TOP_MID, 0, 42);

  climate_status = lv_label_create(home_page);
  set_climate_label(climate_status, "Waiting for DHT12", get_font_14(), lv_color_hex(0xe4ffff));
  lv_obj_set_style_text_align(climate_status, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(climate_status, LV_ALIGN_TOP_MID, 0, 102);

  lv_obj_t* humidity_card = lv_obj_create(home_page);
  lv_obj_set_size(humidity_card, 220, 82);
  lv_obj_align(humidity_card, LV_ALIGN_TOP_LEFT, 10, 135);
  lv_obj_set_style_bg_color(humidity_card, lv_color_hex(0x5e9bc8), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(humidity_card, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_radius(humidity_card, 4, LV_PART_MAIN);
  lv_obj_set_style_border_width(humidity_card, 0, LV_PART_MAIN);
  lv_obj_clear_flag(humidity_card, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t* humidity_title = lv_label_create(humidity_card);
  set_climate_label(humidity_title, "Humidity", get_font_16(), lv_color_hex(0xb9ecff));
  lv_obj_align(humidity_title, LV_ALIGN_TOP_LEFT, 12, 10);

  climate_humidity = lv_label_create(humidity_card);
  set_climate_label(climate_humidity, "--.-%", get_font_20(), lv_color_white());
  lv_obj_align(climate_humidity, LV_ALIGN_TOP_RIGHT, -12, 8);

  lv_obj_t* hint = lv_label_create(home_page);
  set_climate_label(hint, "DHT12 sensor", get_font_12(), lv_color_hex(0xb9ecff));
  lv_obj_align(hint, LV_ALIGN_BOTTOM_MID, 0, -30);
}

void update_climate() {
  float temperature = 0.0f;
  float humidity = 0.0f;
  if (!read_dht12(temperature, humidity)) {
    set_climate_label(climate_status, "DHT12 read failed", get_font_14(),
                      lv_color_hex(0xffd0d0));
    Serial.println("DHT12 read failed");
    return;
  }

  char temperature_text[16];
  char humidity_text[16];
  snprintf(temperature_text, sizeof(temperature_text), "%.1f°", temperature);
  snprintf(humidity_text, sizeof(humidity_text), "%.1f%%", humidity);
  set_climate_label(climate_temperature, temperature_text, get_font_42(),
                    lv_color_white());
  set_climate_label(climate_humidity, humidity_text, get_font_20(),
                    lv_color_white());
  set_climate_label(climate_status, "Indoor conditions", get_font_14(),
                    lv_color_hex(0xe4ffff));
}
