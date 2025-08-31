#pragma once

#include "BleKeyboard.h"
#include "types.h"

constexpr const char* VERSION = "1.0.0";

// Server config
constexpr const char* SERVER_SSID = "Miniboard AP";
constexpr const char* SERVER_PASSWORD = "jebac_zydow42069";

constexpr int SLEEP_TIMEOUT = 15 * 60 * 1000, // 15 minutes
              BATTERY_UPDATE_INTERVAL = 10 * 1000; // 10 seconds

constexpr int PIN_BATTERY_LEVEL = 33,
              PIN_SERVER_BUTTON = 15,
              PIN_LED_BUILTIN = 2;

// buttons 1, 2, 3, 4 -> GPIOs 26, 25, 14, 27
// which means the connection mapping looks like this:
// 3, 4, 1, 2 (the gpios are in order: 14, 27, 26, 25 on the esp32 board)
constexpr int PIN_BUTTONS[] = {
   14, 27, 26, 25
};

constexpr eeprom_settings_t DEFAULT_SETTINGS = {
    BOOT_BLE_KEYBOARD,
    {KEY_DOWN_ARROW, KEY_UP_ARROW, KEY_BACKSPACE, KEY_RETURN},
    0xFF
};