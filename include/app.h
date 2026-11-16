#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <XPT2046_Touchscreen.h>
#include <WiFiManager.h>
#include <lvgl.h>
#include <time.h>

#include "translations.h"

#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33
#define DHT12_SDA 27
#define DHT12_SCL 22
#define DHT12_ADDRESS 0x5C
#define DHT12_UPDATE_INTERVAL 5000UL
#define TEMPERATURE_TOLERANCE -1.0f
#define LCD_BACKLIGHT_PIN 21
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 20 * (LV_COLOR_DEPTH / 8))
#define LATITUDE_DEFAULT "51.5074"
#define LONGITUDE_DEFAULT "-0.1278"
#define LOCATION_DEFAULT "London"
#define DEFAULT_CAPTIVE_SSID "Aura"
#define UPDATE_INTERVAL 600000UL
#define NIGHT_MODE_START_HOUR 22
#define NIGHT_MODE_END_HOUR 6

// Blynk Configuration (replace with your credentials from Blynk.Cloud)
#define BLYNK_TEMPLATE_ID "TMPL6KHyeoQuM"
#define BLYNK_TEMPLATE_NAME "Home Station"
#define BLYNK_AUTH_TOKEN "pYMJE7W3_LdNyHB2704a9CMam85teQiP"
#define BLYNK_PIN_TEMP V0
#define BLYNK_PIN_HUM  V1

LV_FONT_DECLARE(lv_font_montserrat_latin_12);
LV_FONT_DECLARE(lv_font_montserrat_latin_14);
LV_FONT_DECLARE(lv_font_montserrat_latin_16);
LV_FONT_DECLARE(lv_font_montserrat_latin_20);
LV_FONT_DECLARE(lv_font_montserrat_latin_42);

#define DECLARE_WEATHER_ASSET(kind, name) LV_IMG_DECLARE(kind##_##name)
#define DECLARE_WEATHER_ASSETS(kind) \
  DECLARE_WEATHER_ASSET(kind, blizzard); \
  DECLARE_WEATHER_ASSET(kind, blowing_snow); \
  DECLARE_WEATHER_ASSET(kind, clear_night); \
  DECLARE_WEATHER_ASSET(kind, cloudy); \
  DECLARE_WEATHER_ASSET(kind, drizzle); \
  DECLARE_WEATHER_ASSET(kind, flurries); \
  DECLARE_WEATHER_ASSET(kind, haze_fog_dust_smoke); \
  DECLARE_WEATHER_ASSET(kind, heavy_rain); \
  DECLARE_WEATHER_ASSET(kind, heavy_snow); \
  DECLARE_WEATHER_ASSET(kind, isolated_scattered_tstorms_day); \
  DECLARE_WEATHER_ASSET(kind, isolated_scattered_tstorms_night); \
  DECLARE_WEATHER_ASSET(kind, mostly_clear_night); \
  DECLARE_WEATHER_ASSET(kind, mostly_cloudy_day); \
  DECLARE_WEATHER_ASSET(kind, mostly_cloudy_night); \
  DECLARE_WEATHER_ASSET(kind, mostly_sunny); \
  DECLARE_WEATHER_ASSET(kind, partly_cloudy); \
  DECLARE_WEATHER_ASSET(kind, partly_cloudy_night); \
  DECLARE_WEATHER_ASSET(kind, scattered_showers_day); \
  DECLARE_WEATHER_ASSET(kind, scattered_showers_night); \
  DECLARE_WEATHER_ASSET(kind, showers_rain); \
  DECLARE_WEATHER_ASSET(kind, sleet_hail); \
  DECLARE_WEATHER_ASSET(kind, snow_showers_snow); \
  DECLARE_WEATHER_ASSET(kind, strong_tstorms); \
  DECLARE_WEATHER_ASSET(kind, sunny); \
  DECLARE_WEATHER_ASSET(kind, tornado); \
  DECLARE_WEATHER_ASSET(kind, wintry_mix_rain_snow)

DECLARE_WEATHER_ASSETS(icon);
DECLARE_WEATHER_ASSETS(image);
#undef DECLARE_WEATHER_ASSETS
#undef DECLARE_WEATHER_ASSET

extern Language current_language;
extern SPIClass touchscreenSPI;
extern XPT2046_Touchscreen touchscreen;
extern uint32_t draw_buf[DRAW_BUF_SIZE / 4];
extern int x;
extern int y;
extern int z;
extern Preferences prefs;
extern bool use_fahrenheit;
extern bool use_24_hour;
extern bool use_night_mode;
extern char latitude[16];
extern char longitude[16];
extern String location;
extern char dd_opts[512];
extern DynamicJsonDocument geoDoc;
extern JsonArray geoResults;
extern bool night_mode_active;
extern bool temp_screen_wakeup_active;
extern lv_timer_t* temp_screen_wakeup_timer;
extern lv_obj_t* lbl_today_temp;
extern lv_obj_t* lbl_today_feels_like;
extern lv_obj_t* img_today_icon;
extern lv_obj_t* lbl_forecast;
extern lv_obj_t* box_daily;
extern lv_obj_t* box_hourly;
extern lv_obj_t* lbl_daily_day[7];
extern lv_obj_t* lbl_daily_high[7];
extern lv_obj_t* lbl_daily_low[7];
extern lv_obj_t* img_daily[7];
extern lv_obj_t* lbl_hourly[7];
extern lv_obj_t* lbl_precipitation_probability[7];
extern lv_obj_t* lbl_hourly_temp[7];
extern lv_obj_t* img_hourly[7];
extern lv_obj_t* lbl_loc;
extern lv_obj_t* loc_ta;
extern lv_obj_t* results_dd;
extern lv_obj_t* btn_close_loc;
extern lv_obj_t* btn_close_obj;
extern lv_obj_t* kb;
extern lv_obj_t* settings_win;
extern lv_obj_t* location_win;
extern lv_obj_t* unit_switch;
extern lv_obj_t* clock_24hr_switch;
extern lv_obj_t* night_mode_switch;
extern lv_obj_t* language_dropdown;
extern lv_obj_t* lbl_clock;
extern lv_obj_t* home_page;
extern lv_obj_t* navigation_bar;
extern lv_obj_t* climate_temperature;
extern lv_obj_t* climate_humidity;
extern lv_obj_t* climate_status;

const lv_font_t* get_font_12();
const lv_font_t* get_font_14();
const lv_font_t* get_font_16();
const lv_font_t* get_font_20();
const lv_font_t* get_font_42();
void touchscreen_read(lv_indev_t* indev, lv_indev_data_t* data);
void setup();
void loop();
void flush_wifi_splashscreen(uint32_t ms = 200);
void apModeCallback(WiFiManager* mgr);
void wifi_splash_screen();
void create_ui();
void create_climate_ui();
void update_climate();
String urlencode(const String& str);
void daily_cb(lv_event_t* event);
void hourly_cb(lv_event_t* event);
void create_settings_window();
void create_location_dialog();
void do_geocode_query(const char* query);
void fetch_and_update_weather();
void update_clock(lv_timer_t* timer);
bool night_mode_should_be_active();
void activate_night_mode();
void deactivate_night_mode();
void check_for_night_mode();
void handle_temp_screen_wakeup_timeout(lv_timer_t* timer);
const lv_img_dsc_t* choose_image(int code, int is_day);
const lv_img_dsc_t* choose_icon(int code, int is_day);

// Blynk functions
void init_blynk();
void run_blynk();
void blynk_send_climate(float temperature, float humidity);
