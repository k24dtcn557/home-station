#include "app.h"

Language current_language = LANG_EN;

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);
uint32_t draw_buf[DRAW_BUF_SIZE / 4];
int x, y, z;

// Preferences
Preferences prefs;
bool use_fahrenheit = false;
bool use_24_hour = false;
bool use_night_mode = false;
char latitude[16] = LATITUDE_DEFAULT;
char longitude[16] = LONGITUDE_DEFAULT;
String location = String(LOCATION_DEFAULT);
char dd_opts[512];
DynamicJsonDocument geoDoc(8 * 1024);
JsonArray geoResults;

// Screen dimming variables
bool night_mode_active = false;
bool temp_screen_wakeup_active = false;
lv_timer_t *temp_screen_wakeup_timer = nullptr;

// UI components
lv_obj_t *lbl_today_temp;
lv_obj_t *lbl_today_feels_like;
lv_obj_t *img_today_icon;
lv_obj_t *lbl_forecast;
lv_obj_t *box_daily;
lv_obj_t *box_hourly;
lv_obj_t *lbl_daily_day[7];
lv_obj_t *lbl_daily_high[7];
lv_obj_t *lbl_daily_low[7];
lv_obj_t *img_daily[7];
lv_obj_t *lbl_hourly[7];
lv_obj_t *lbl_precipitation_probability[7];
lv_obj_t *lbl_hourly_temp[7];
lv_obj_t *img_hourly[7];
lv_obj_t *lbl_loc;
lv_obj_t *loc_ta;
lv_obj_t *results_dd;
lv_obj_t *btn_close_loc;
lv_obj_t *btn_close_obj;
lv_obj_t *kb;
lv_obj_t *settings_win;
lv_obj_t *location_win = nullptr;
lv_obj_t *unit_switch;
lv_obj_t *clock_24hr_switch;
lv_obj_t *night_mode_switch;
lv_obj_t *language_dropdown;
lv_obj_t *lbl_clock;
lv_obj_t *home_page;
lv_obj_t *navigation_bar;
lv_obj_t *climate_temperature;
lv_obj_t *climate_humidity;
lv_obj_t *climate_status;

// Weather icons
LV_IMG_DECLARE(icon_blizzard);
LV_IMG_DECLARE(icon_blowing_snow);
LV_IMG_DECLARE(icon_clear_night);
LV_IMG_DECLARE(icon_cloudy);
LV_IMG_DECLARE(icon_drizzle);
LV_IMG_DECLARE(icon_flurries);
LV_IMG_DECLARE(icon_haze_fog_dust_smoke);
LV_IMG_DECLARE(icon_heavy_rain);
LV_IMG_DECLARE(icon_heavy_snow);
LV_IMG_DECLARE(icon_isolated_scattered_tstorms_day);
LV_IMG_DECLARE(icon_isolated_scattered_tstorms_night);
LV_IMG_DECLARE(icon_mostly_clear_night);
LV_IMG_DECLARE(icon_mostly_cloudy_day);
LV_IMG_DECLARE(icon_mostly_cloudy_night);
LV_IMG_DECLARE(icon_mostly_sunny);
LV_IMG_DECLARE(icon_partly_cloudy);
LV_IMG_DECLARE(icon_partly_cloudy_night);
LV_IMG_DECLARE(icon_scattered_showers_day);
LV_IMG_DECLARE(icon_scattered_showers_night);
LV_IMG_DECLARE(icon_showers_rain);
LV_IMG_DECLARE(icon_sleet_hail);
LV_IMG_DECLARE(icon_snow_showers_snow);
LV_IMG_DECLARE(icon_strong_tstorms);
LV_IMG_DECLARE(icon_sunny);
LV_IMG_DECLARE(icon_tornado);
LV_IMG_DECLARE(icon_wintry_mix_rain_snow);

// Weather Images
LV_IMG_DECLARE(image_blizzard);
LV_IMG_DECLARE(image_blowing_snow);
LV_IMG_DECLARE(image_clear_night);
LV_IMG_DECLARE(image_cloudy);
LV_IMG_DECLARE(image_drizzle);
LV_IMG_DECLARE(image_flurries);
LV_IMG_DECLARE(image_haze_fog_dust_smoke);
LV_IMG_DECLARE(image_heavy_rain);
LV_IMG_DECLARE(image_heavy_snow);
LV_IMG_DECLARE(image_isolated_scattered_tstorms_day);
LV_IMG_DECLARE(image_isolated_scattered_tstorms_night);
LV_IMG_DECLARE(image_mostly_clear_night);
LV_IMG_DECLARE(image_mostly_cloudy_day);
LV_IMG_DECLARE(image_mostly_cloudy_night);
LV_IMG_DECLARE(image_mostly_sunny);
LV_IMG_DECLARE(image_partly_cloudy);
LV_IMG_DECLARE(image_partly_cloudy_night);
LV_IMG_DECLARE(image_scattered_showers_day);
LV_IMG_DECLARE(image_scattered_showers_night);
LV_IMG_DECLARE(image_showers_rain);
LV_IMG_DECLARE(image_sleet_hail);
LV_IMG_DECLARE(image_snow_showers_snow);
LV_IMG_DECLARE(image_strong_tstorms);
LV_IMG_DECLARE(image_sunny);
LV_IMG_DECLARE(image_tornado);
LV_IMG_DECLARE(image_wintry_mix_rain_snow);


void setup() {
  Serial.begin(115200);
  delay(100);

  TFT_eSPI tft = TFT_eSPI();
  tft.init();
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);

  lv_init();
  Wire.begin(DHT12_SDA, DHT12_SCL);

  // Init touchscreen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(0);

  lv_display_t *disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touchscreen_read);

  // Load saved prefs
  prefs.begin("weather", false);
  String lat = prefs.getString("latitude", LATITUDE_DEFAULT);
  lat.toCharArray(latitude, sizeof(latitude));
  String lon = prefs.getString("longitude", LONGITUDE_DEFAULT);
  lon.toCharArray(longitude, sizeof(longitude));
  use_fahrenheit = prefs.getBool("useFahrenheit", false);
  location = prefs.getString("location", LOCATION_DEFAULT);
  use_night_mode = prefs.getBool("useNightMode", false);
  uint32_t brightness = prefs.getUInt("brightness", 255);
  use_24_hour = prefs.getBool("use24Hour", false);
  current_language = (Language)prefs.getUInt("language", LANG_EN);
  analogWrite(LCD_BACKLIGHT_PIN, brightness);

  // Check for Wi-Fi config and request it if not available
  WiFiManager wm;
  wm.setAPCallback(apModeCallback);
  wm.autoConnect(DEFAULT_CAPTIVE_SSID);

  lv_timer_create(update_clock, 1000, NULL);

  lv_obj_clean(lv_scr_act());
  create_ui();
  update_climate();
  fetch_and_update_weather();
}

void flush_wifi_splashscreen(uint32_t ms) {
  uint32_t start = millis();
  while (millis() - start < ms) {
    lv_timer_handler();
    delay(5);
  }
}

void apModeCallback(WiFiManager *mgr) {
  wifi_splash_screen();
  flush_wifi_splashscreen();
}

void loop() {
  lv_timer_handler();
  static uint32_t last = millis();

  static uint32_t last_climate = 0;
  if (millis() - last_climate >= DHT12_UPDATE_INTERVAL) {
    update_climate();
    last_climate = millis();
  }

  if (millis() - last >= UPDATE_INTERVAL) {
    fetch_and_update_weather();
    last = millis();
  }

  lv_tick_inc(5);
  delay(5);
}
