#include "src/keyboard.h"
#include "src/server.h"
#include <EEPROM.h>

/*

Bluetooth Keyboard

Made by IlikeChooros
License: MIT

*/

typedef void (*task_t)();

task_t loopTask = nullptr;
EEPROMSettings settings;

void printConfig(const config_t& config) {
    Serial.println("Miniboard config");
    Serial.printf("XTAL: %dMhz\n", getXtalFrequencyMhz());
    Serial.printf("CPU: %dMhz\n", getCpuFrequencyMhz());
    Serial.printf("Boot type: %s\n", config.boot_type == BOOT_BLE_KEYBOARD ? "BLE Keyboard" : "HTTP Server");
     if (config.boot_type == BOOT_HTTP_SERVER) {
        Serial.printf("AP IP: %s\n", config.ap_ip.toString().c_str());
    }
    
    auto keypad = config.keypad;
    for (int i = 0; i < sizeof(keypad) / sizeof(keypad[0]); ++i) {
        Serial.printf("Keypad button %d: %d\n", i + 1, keypad[i]);
    }
}

void setupLed() {
    gpio_config_t led_conf = {
        .pin_bit_mask = (1ULL << PIN_LED_BUILTIN),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&led_conf);
}

void blinkLed(int times, int delayms = 300) {
    for (int i = 0; i < times; i++) {
        gpio_set_level(gpio_num_t(PIN_LED_BUILTIN), HIGH);
        delay(delayms);
        gpio_set_level(gpio_num_t(PIN_LED_BUILTIN), LOW);
        delay(delayms);
    }
}

// Setup button callbacks and wakeup protocols
void setup() {
    // Read EEPROM for saved settings
    settings.load();
    setupLed();
    Serial.begin(115200);
    config_t config = {};
    
    switch (settings.get()->boot_type)
    {
    case BOOT_HTTP_SERVER:
        blinkLed(2);
        config.ap_ip = startServer(settings);
        loopTask = serverTask;
        break;

    case BOOT_BLE_KEYBOARD:
    default:
        blinkLed(1);
        setupKeyboard(settings);
        loopTask = keyboardTask;
    }

    config = *settings.get();
    printConfig(config);
}

void loop() {
    loopTask();
}