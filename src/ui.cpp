#include "ui.h"

#include <Arduino.h>
#include <PNGdec.h>
#include <new>
#include <string.h>

namespace {

constexpr lv_coord_t kTabBarWidth = 88;
constexpr uint16_t kWeatherIconSize = 64;
constexpr lv_coord_t kWeatherIconDisplaySize = 40;

PNG* weather_png = nullptr;
lv_obj_t* weather_icon = nullptr;
lv_obj_t* weather_icon_placeholder = nullptr;
lv_obj_t* weather_temperature = nullptr;
lv_obj_t* weather_condition = nullptr;
lv_obj_t* sensor_reading = nullptr;
lv_obj_t* system_status = nullptr;
lv_img_dsc_t weather_icon_dsc;
lv_color_t weather_icon_pixels[kWeatherIconSize * kWeatherIconSize];

void style_tab_buttons(lv_obj_t* tabview) {
  lv_obj_t* tab_buttons = lv_tabview_get_tab_btns(tabview);
  lv_obj_set_style_bg_color(
      tab_buttons, lv_color_hex(0x0F172A), LV_PART_MAIN);
  lv_obj_set_style_border_width(tab_buttons, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(tab_buttons, 6, LV_PART_MAIN);
  lv_obj_set_style_bg_color(
      tab_buttons, lv_color_hex(0x1E293B), LV_PART_ITEMS);
  lv_obj_set_style_text_color(
      tab_buttons, lv_color_hex(0x94A3B8), LV_PART_ITEMS);
  lv_obj_set_style_bg_color(
      tab_buttons,
      lv_color_hex(0x2563EB),
      LV_PART_ITEMS | LV_STATE_CHECKED);
  lv_obj_set_style_text_color(
      tab_buttons,
      lv_color_white(),
      LV_PART_ITEMS | LV_STATE_CHECKED);
  lv_obj_set_style_radius(tab_buttons, 8, LV_PART_ITEMS);
  lv_obj_set_style_pad_ver(tab_buttons, 4, LV_PART_ITEMS);
  lv_obj_set_style_pad_hor(tab_buttons, 6, LV_PART_ITEMS);
}

void style_page(lv_obj_t* page) {
  lv_obj_set_style_bg_color(page, lv_color_hex(0xF8FAFC), 0);
  lv_obj_set_style_border_width(page, 0, 0);
  lv_obj_set_style_pad_all(page, 0, 0);
}

void create_page_header(
    lv_obj_t* page,
    const char* title,
    const char* subtitle) {
  lv_obj_t* label = lv_label_create(page);
  lv_label_set_text(label, title);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0x0F172A), 0);
  lv_obj_align(label, LV_ALIGN_TOP_LEFT, 16, 14);

  lv_obj_t* description = lv_label_create(page);
  lv_label_set_text(description, subtitle);
  lv_obj_set_style_text_color(description, lv_color_hex(0x64748B), 0);
  lv_obj_align(description, LV_ALIGN_TOP_LEFT, 16, 36);
}

void decode_weather_icon_line(PNGDRAW* draw) {
  if (draw->y >= kWeatherIconSize || draw->iWidth > kWeatherIconSize) {
    return;
  }

  uint16_t line[kWeatherIconSize];
  if (weather_png == nullptr) {
    return;
  }

  weather_png->getLineAsRGB565(
      draw, line, PNG_RGB565_LITTLE_ENDIAN, 0xFFFFFFFF);
  memcpy(
      &weather_icon_pixels[draw->y * kWeatherIconSize],
      line,
      draw->iWidth * sizeof(uint16_t));
}

bool set_weather_icon(const uint8_t* icon_data, size_t icon_size) {
  if (icon_data == nullptr || icon_size == 0) {
    return false;
  }

  weather_png = new (std::nothrow) PNG();
  if (weather_png == nullptr ||
      weather_png->openRAM(
          const_cast<uint8_t*>(icon_data),
          icon_size,
          decode_weather_icon_line) != PNG_SUCCESS) {
    delete weather_png;
    weather_png = nullptr;
    return false;
  }

  const bool valid_size =
      weather_png->getWidth() <= kWeatherIconSize &&
      weather_png->getHeight() <= kWeatherIconSize;
  const bool decoded =
      valid_size && weather_png->decode(nullptr, 0) == PNG_SUCCESS;
  if (decoded) {
    weather_icon_dsc.header.always_zero = 0;
    weather_icon_dsc.header.w = kWeatherIconSize;
    weather_icon_dsc.header.h = kWeatherIconSize;
    weather_icon_dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
    weather_icon_dsc.data_size = sizeof(weather_icon_pixels);
    weather_icon_dsc.data =
        reinterpret_cast<const uint8_t*>(weather_icon_pixels);
    lv_img_set_src(weather_icon, &weather_icon_dsc);
    lv_img_set_zoom(
        weather_icon,
        kWeatherIconDisplaySize * LV_IMG_ZOOM_NONE / kWeatherIconSize);
    lv_obj_clear_flag(weather_icon, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(weather_icon_placeholder, LV_OBJ_FLAG_HIDDEN);
  } else {
    Serial.printf(
        "[Weather] Icon decode failed (size: %d x %d)\n",
        weather_png->getWidth(),
        weather_png->getHeight());
  }

  weather_png->close();
  delete weather_png;
  weather_png = nullptr;
  return decoded;
}

lv_obj_t* create_card(lv_obj_t* page, lv_coord_t y) {
  lv_obj_t* card = lv_obj_create(page);
  lv_obj_set_size(card, 184, 66);
  lv_obj_align(card, LV_ALIGN_TOP_LEFT, 16, y);
  lv_obj_set_style_bg_color(card, lv_color_white(), 0);
  lv_obj_set_style_border_width(card, 0, 0);
  lv_obj_set_style_radius(card, 12, 0);
  lv_obj_set_style_pad_all(card, 12, 0);
  lv_obj_set_style_shadow_width(card, 12, 0);
  lv_obj_set_style_shadow_opa(card, LV_OPA_10, 0);
  lv_obj_set_style_shadow_color(card, lv_color_hex(0x0F172A), 0);
  return card;
}

void create_status_card(
    lv_obj_t* page,
    const char* heading,
    const char* value,
    lv_color_t accent,
    lv_coord_t y) {
  lv_obj_t* card = create_card(page, y);

  lv_obj_t* heading_label = lv_label_create(card);
  lv_label_set_text(heading_label, heading);
  lv_obj_set_style_text_color(heading_label, lv_color_hex(0x64748B), 0);
  lv_obj_align(heading_label, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_obj_t* value_label = lv_label_create(card);
  lv_label_set_text(value_label, value);
  lv_obj_set_style_text_color(value_label, accent, 0);
  lv_obj_align(value_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  if (strcmp(heading, "System status") == 0) {
    system_status = value_label;
  }
}

void create_compact_status_card(
    lv_obj_t* page,
    const char* value,
    lv_coord_t y) {
  lv_obj_t* card = create_card(page, y);
  lv_obj_set_height(card, 44);

  system_status = lv_label_create(card);
  lv_label_set_text(system_status, value);
  lv_obj_set_style_text_color(system_status, lv_color_hex(0x16A34A), 0);
  lv_obj_center(system_status);
}

void create_sensor_card(lv_obj_t* page) {
  lv_obj_t* card = create_card(page, 148);

  sensor_reading = lv_label_create(card);
  lv_label_set_text(sensor_reading, "DHT12: --.- C  --.- %");
  lv_obj_set_style_text_font(sensor_reading, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(sensor_reading, lv_color_hex(0x0F172A), 0);
  lv_obj_center(sensor_reading);
}

void create_home_page(lv_obj_t* page) {
  style_page(page);
  create_page_header(page, "Home", "Your home at a glance");
  lv_obj_t* weather_card = create_card(page, 72);

  weather_icon_placeholder = lv_obj_create(weather_card);
  lv_obj_set_size(weather_icon_placeholder, 40, 40);
  lv_obj_set_style_bg_color(
      weather_icon_placeholder, lv_color_hex(0xFBBF24), 0);
  lv_obj_set_style_border_width(weather_icon_placeholder, 0, 0);
  lv_obj_set_style_radius(weather_icon_placeholder, LV_RADIUS_CIRCLE, 0);
  lv_obj_align(weather_icon_placeholder, LV_ALIGN_LEFT_MID, 0, 0);

  lv_obj_t* placeholder_label = lv_label_create(weather_icon_placeholder);
  lv_label_set_text(placeholder_label, "*");
  lv_obj_set_style_text_color(placeholder_label, lv_color_white(), 0);
  lv_obj_center(placeholder_label);

  weather_icon = lv_img_create(weather_card);
  lv_obj_set_size(
      weather_icon, kWeatherIconDisplaySize, kWeatherIconDisplaySize);
  lv_obj_add_flag(weather_icon, LV_OBJ_FLAG_HIDDEN);
  lv_obj_align(weather_icon, LV_ALIGN_LEFT_MID, 0, 0);

  weather_temperature = lv_label_create(weather_card);
  lv_label_set_text(weather_temperature, "-- C");
  lv_obj_set_style_text_font(
      weather_temperature, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(
      weather_temperature, lv_color_hex(0x0F172A), 0);
  lv_obj_align(weather_temperature, LV_ALIGN_TOP_LEFT, 58, 0);

  weather_condition = lv_label_create(weather_card);
  lv_label_set_text(weather_condition, "Loading weather...");
  lv_obj_set_width(weather_condition, 104);
  lv_label_set_long_mode(weather_condition, LV_LABEL_LONG_DOT);
  lv_obj_set_style_text_color(
      weather_condition, lv_color_hex(0x64748B), 0);
  lv_obj_align(weather_condition, LV_ALIGN_BOTTOM_LEFT, 58, 0);

  create_sensor_card(page);
  create_compact_status_card(page, "Ready", 196);
}

void create_wifi_page(lv_obj_t* page) {
  style_page(page);
  create_page_header(page, "Wi-Fi", "Network connection");
  create_status_card(
      page, "Network", "Not connected", lv_color_hex(0xF97316), 72);
}

}  // namespace

void ui_init() {
  lv_obj_t* screen = lv_scr_act();
  lv_obj_set_style_bg_color(screen, lv_color_hex(0xF8FAFC), 0);

  lv_obj_t* tabview = lv_tabview_create(screen, LV_DIR_RIGHT, kTabBarWidth);
  lv_obj_set_style_bg_color(tabview, lv_color_hex(0xF8FAFC), LV_PART_MAIN);
  lv_obj_set_style_pad_all(tabview, 0, LV_PART_MAIN);

  lv_obj_t* home_page = lv_tabview_add_tab(tabview, "Home");
  lv_obj_t* wifi_page = lv_tabview_add_tab(tabview, "Wifi");

  create_home_page(home_page);
  create_wifi_page(wifi_page);
  style_tab_buttons(tabview);
}

void ui_set_weather_status(const char* status) {
  if (weather_condition != nullptr) {
    lv_label_set_text(weather_condition, status);
  }
}

void ui_set_system_status(const char* status) {
  if (system_status != nullptr) {
    lv_label_set_text(system_status, status);
  }
}

void ui_set_sensor_status(const char* status) {
  if (sensor_reading != nullptr) {
    lv_label_set_text(sensor_reading, status);
  }
}

void ui_set_sensor(float temperature_c, float humidity_percent) {
  if (sensor_reading != nullptr) {
    String reading = "DHT12: ";
    reading += String(temperature_c, 1);
    reading += " C  ";
    reading += String(humidity_percent, 1);
    reading += " %";
    lv_label_set_text(sensor_reading, reading.c_str());
  }
}

void ui_set_weather(
    float temperature_c,
    const char* condition,
    const uint8_t* icon_data,
    size_t icon_size) {
  if (weather_temperature != nullptr) {
    String temperature_text = String(temperature_c, 1);
    temperature_text += " C";
    lv_label_set_text(weather_temperature, temperature_text.c_str());
  }
  ui_set_weather_status(condition);
  set_weather_icon(icon_data, icon_size);
}
