#pragma once

#include <stddef.h>
#include <stdint.h>

#include <lvgl.h>

void ui_init();
void ui_set_weather_status(const char* status);
void ui_set_system_status(const char* status);
void ui_set_weather(
    float temperature_c,
    const char* condition,
    const uint8_t* icon_data,
    size_t icon_size);
