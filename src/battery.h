#pragma once

#include "config.h"
#include "log.h"


inline void setupBattery() {
    // battery level pin
    gpio_config_t battery_conf = {
        .pin_bit_mask = (1ULL << PIN_BATTERY_LEVEL),
        .mode = GPIO_MODE_INPUT,
    };
    gpio_config(&battery_conf);
}

/**
 * Returns battery level as a percentage (0-100).
 * Using 47 + 100 k ohm voltage divider, with li ion 3.7v battery,
 * so 0% is about 3.2v and 100% is about 4.1v, with that voltage divider
 * I get 100/147 * 3.2v = 2.176v, and 100/147 * 4.1v = 2.789v
 */
inline uint8_t readBatteryLevel() {
    constexpr int BATTERY_LEVEL_MIN_MV = 2176,
                  BATTERY_LEVEL_MAX_MV = 2789;

    int rawMv = analogReadMilliVolts(PIN_BATTERY_LEVEL);
    int mapped = std::max(BATTERY_LEVEL_MIN_MV, std::min(BATTERY_LEVEL_MAX_MV, rawMv));
    return uint8_t(float(mapped - BATTERY_LEVEL_MIN_MV) / float(BATTERY_LEVEL_MAX_MV - BATTERY_LEVEL_MIN_MV) * 100.0f);
}