#include "app.h"

const lv_font_t* get_font_12() {
  return &lv_font_montserrat_latin_12;
}

const lv_font_t* get_font_14() {
  return &lv_font_montserrat_latin_14;
}

const lv_font_t* get_font_16() {
  return &lv_font_montserrat_latin_16;
}

const lv_font_t* get_font_20() {
  return &lv_font_montserrat_latin_20;
}

const lv_font_t* get_font_42() {
  return &lv_font_montserrat_latin_42;
}

void wifi_splash_screen() {
  lv_obj_t *scr = lv_scr_act();
  lv_obj_clean(scr);
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x4c8cb9), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_color(scr, lv_color_hex(0xa6cdec), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_dir(scr, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);

  const LocalizedStrings* strings = get_strings(current_language);
  lv_obj_t *lbl = lv_label_create(scr);
  lv_label_set_text(lbl, strings->wifi_config);
  lv_obj_set_style_text_font(lbl, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_center(lbl);
  lv_scr_load(scr);
}

static void navigation_event_cb(lv_event_t* event);

void create_ui() {
  lv_obj_t *scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x4c8cb9), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_color(scr, lv_color_hex(0xa6cdec), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_dir(scr, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);

  img_today_icon = lv_img_create(scr);
  lv_img_set_src(img_today_icon, &image_partly_cloudy);
  lv_obj_align(img_today_icon, LV_ALIGN_TOP_MID, -64, 4);

  static lv_style_t default_label_style;
  lv_style_init(&default_label_style);
  lv_style_set_text_color(&default_label_style, lv_color_hex(0xFFFFFF));
  lv_style_set_text_opa(&default_label_style, LV_OPA_COVER);

  const LocalizedStrings* strings = get_strings(current_language);

  lbl_today_temp = lv_label_create(scr);
  lv_label_set_text(lbl_today_temp, strings->temp_placeholder);
  lv_obj_set_style_text_font(lbl_today_temp, get_font_42(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_today_temp, LV_ALIGN_TOP_MID, 45, 25);
  lv_obj_add_style(lbl_today_temp, &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);

  lbl_today_feels_like = lv_label_create(scr);
  lv_label_set_text(lbl_today_feels_like, strings->feels_like_temp);
  lv_obj_set_style_text_font(lbl_today_feels_like, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(lbl_today_feels_like, lv_color_hex(0xe4ffff), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_today_feels_like, LV_ALIGN_TOP_MID, 45, 75);

  lbl_forecast = lv_label_create(scr);
  lv_label_set_text(lbl_forecast, strings->seven_day_forecast);
  lv_obj_set_style_text_font(lbl_forecast, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(lbl_forecast, lv_color_hex(0xe4ffff), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_forecast, LV_ALIGN_TOP_LEFT, 20, 100);

  box_daily = lv_obj_create(scr);
  lv_obj_set_size(box_daily, 220, 180);
  lv_obj_align(box_daily, LV_ALIGN_TOP_LEFT, 10, 115);
  lv_obj_set_style_bg_color(box_daily, lv_color_hex(0x5e9bc8), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(box_daily, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_radius(box_daily, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(box_daily, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_clear_flag(box_daily, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(box_daily, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_pad_all(box_daily, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_gap(box_daily, 0, LV_PART_MAIN);
  lv_obj_add_event_cb(box_daily, daily_cb, LV_EVENT_CLICKED, NULL);

  for (int i = 0; i < 7; i++) {
    lbl_daily_day[i] = lv_label_create(box_daily);
    lbl_daily_high[i] = lv_label_create(box_daily);
    lbl_daily_low[i] = lv_label_create(box_daily);
    img_daily[i] = lv_img_create(box_daily);

    lv_obj_add_style(lbl_daily_day[i], &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_daily_day[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_daily_day[i], LV_ALIGN_TOP_LEFT, 2, i * 24);

    lv_obj_add_style(lbl_daily_high[i], &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_daily_high[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_daily_high[i], LV_ALIGN_TOP_RIGHT, 0, i * 24);

    lv_label_set_text(lbl_daily_low[i], "");
    lv_obj_set_style_text_color(lbl_daily_low[i], lv_color_hex(0xb9ecff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_daily_low[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_daily_low[i], LV_ALIGN_TOP_RIGHT, -50, i * 24);

    lv_img_set_src(img_daily[i], &icon_partly_cloudy);
    lv_obj_align(img_daily[i], LV_ALIGN_TOP_LEFT, 72, i * 24);
  }

  box_hourly = lv_obj_create(scr);
  lv_obj_set_size(box_hourly, 220, 180);
  lv_obj_align(box_hourly, LV_ALIGN_TOP_LEFT, 10, 115);
  lv_obj_set_style_bg_color(box_hourly, lv_color_hex(0x5e9bc8), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(box_hourly, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_radius(box_hourly, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(box_hourly, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_clear_flag(box_hourly, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(box_hourly, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_pad_all(box_hourly, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_gap(box_hourly, 0, LV_PART_MAIN);
  lv_obj_add_event_cb(box_hourly, hourly_cb, LV_EVENT_CLICKED, NULL);

  for (int i = 0; i < 7; i++) {
    lbl_hourly[i] = lv_label_create(box_hourly);
    lbl_precipitation_probability[i] = lv_label_create(box_hourly);
    lbl_hourly_temp[i] = lv_label_create(box_hourly);
    img_hourly[i] = lv_img_create(box_hourly);

    lv_obj_add_style(lbl_hourly[i], &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_hourly[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_hourly[i], LV_ALIGN_TOP_LEFT, 2, i * 24);

    lv_obj_add_style(lbl_hourly_temp[i], &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_hourly_temp[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_hourly_temp[i], LV_ALIGN_TOP_RIGHT, 0, i * 24);

    lv_label_set_text(lbl_precipitation_probability[i], "");
    lv_obj_set_style_text_color(lbl_precipitation_probability[i], lv_color_hex(0xb9ecff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_precipitation_probability[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_precipitation_probability[i], LV_ALIGN_TOP_RIGHT, -55, i * 24);

    lv_img_set_src(img_hourly[i], &icon_partly_cloudy);
    lv_obj_align(img_hourly[i], LV_ALIGN_TOP_LEFT, 72, i * 24);
  }

  lv_obj_add_flag(box_hourly, LV_OBJ_FLAG_HIDDEN);

  // Create clock label in the top-right corner
  lbl_clock = lv_label_create(scr);
  lv_obj_set_style_text_font(lbl_clock, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(lbl_clock, lv_color_hex(0xb9ecff), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_label_set_text(lbl_clock, "");
  lv_obj_align(lbl_clock, LV_ALIGN_TOP_RIGHT, -10, 2);

  home_page = lv_obj_create(scr);
  lv_obj_set_size(home_page, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_set_style_bg_color(
      home_page, lv_color_hex(0x4c8cb9), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_color(
      home_page, lv_color_hex(0xa6cdec), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_dir(
      home_page, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(
      home_page, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(home_page, 0, LV_PART_MAIN);
  create_climate_ui();
  lv_obj_add_flag(home_page, LV_OBJ_FLAG_HIDDEN);

  navigation_bar = lv_obj_create(scr);
  lv_obj_set_size(navigation_bar, SCREEN_WIDTH, 24);
  lv_obj_align(navigation_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(
      navigation_bar, lv_color_hex(0x1e4f73), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(navigation_bar, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(navigation_bar, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(navigation_bar, 0, LV_PART_MAIN);
  lv_obj_clear_flag(navigation_bar, LV_OBJ_FLAG_SCROLLABLE);

  const char* navigation_labels[] = {"Home", "Weather", "Settings"};
  for (uint8_t index = 0; index < 3; ++index) {
    lv_obj_t* button = lv_btn_create(navigation_bar);
    lv_obj_set_size(button, SCREEN_WIDTH / 3, 24);
    lv_obj_align(button, LV_ALIGN_LEFT_MID, index * (SCREEN_WIDTH / 3), 0);
    lv_obj_set_style_radius(button, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(
        button, lv_color_hex(0x1e4f73), LV_PART_MAIN);
    lv_obj_set_style_bg_color(
        button, lv_color_hex(0x2563a0), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(
        button,
        navigation_event_cb,
        LV_EVENT_CLICKED,
        reinterpret_cast<void*>(static_cast<uintptr_t>(index)));

    lv_obj_t* label = lv_label_create(button);
    lv_label_set_text(label, navigation_labels[index]);
    lv_obj_set_style_text_font(label, get_font_12(), LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(label);
  }
}

void daily_cb(lv_event_t *e) {
  const LocalizedStrings* strings = get_strings(current_language);
  lv_obj_add_flag(box_daily, LV_OBJ_FLAG_HIDDEN);
  lv_label_set_text(lbl_forecast, strings->hourly_forecast);
  lv_obj_clear_flag(box_hourly, LV_OBJ_FLAG_HIDDEN);
}

void hourly_cb(lv_event_t *e) {
  const LocalizedStrings* strings = get_strings(current_language);
  lv_obj_add_flag(box_hourly, LV_OBJ_FLAG_HIDDEN);
  lv_label_set_text(lbl_forecast, strings->seven_day_forecast);
  lv_obj_clear_flag(box_daily, LV_OBJ_FLAG_HIDDEN);
}

static void navigation_event_cb(lv_event_t *e) {
  const uintptr_t page =
      reinterpret_cast<uintptr_t>(lv_event_get_user_data(e));

  if (page == 0) {
    lv_obj_clear_flag(home_page, LV_OBJ_FLAG_HIDDEN);
    return;
  }

  lv_obj_add_flag(home_page, LV_OBJ_FLAG_HIDDEN);
  if (page == 1) {
    return;
  }

  create_settings_window();
}
