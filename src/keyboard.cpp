#include "keyboard.h"

BleKeyboard bleKeyboard;

Button serverButton = Button(PIN_SERVER_BUTTON);

Button keypadButtons[] = {
    Button(PIN_BUTTONS[2]),
    Button(PIN_BUTTONS[3]),
    Button(PIN_BUTTONS[0]),
    Button(PIN_BUTTONS[1])
};

template <uint8_t key>
void press() {
    bleKeyboard.press(key);
}

const internal::callback_t callbacks[] = {
    press<KEY_DOWN_ARROW>,
    press<KEY_UP_ARROW>,
    press<KEY_BACKSPACE>,
    press<KEY_RETURN>
};

void releaseAll() {
    bleKeyboard.releaseAll();
}

/**
 * Read the battery level and set the bt characteristic.
 */
void batteryTask() {
    static int64_t lastUpdate = 0;

    if (millis() - lastUpdate > BATTERY_UPDATE_INTERVAL) {
        lastUpdate = millis();
        bleKeyboard.setBatteryLevel(readBatteryLevel());
    }
}

void setupKeyboard(EEPROMSettings& settings) {

    // No need for max cpu performance, to save power using lower clock speed
    setCpuFrequencyMhz(80);

    // battery level pin
    gpio_config_t battery_conf = {
        .pin_bit_mask = (1ULL << PIN_BATTERY_LEVEL),
        .mode = GPIO_MODE_INPUT,
    };
    gpio_config(&battery_conf);

    bleKeyboard = BleKeyboard("Miniboard", "Pavlov sp. z o.o.", readBatteryLevel());
    bleKeyboard.begin();

    serverButton.begin();

    // Restart the device and boot the HTTP server
    serverButton.onPress([&settings](){
        auto currentSettings = settings.get();
        currentSettings->boot_type = BOOT_HTTP_SERVER;
        settings.save();
        esp_restart();
    });

    for (int i = 0; i < sizeof(keypadButtons) / sizeof(keypadButtons[0]); ++i) {
        keypadButtons[i].begin();
        keypadButtons[i].onPress(callbacks[i]).onRelease(releaseAll);

        // wakeup if any button is pressed
        gpio_wakeup_enable(gpio_num_t(PIN_BUTTONS[i]), GPIO_INTR_LOW_LEVEL);
    }
    
    esp_sleep_enable_gpio_wakeup();
}

void keyboardTask() {
    static int64_t lastActivity = 0;

    // Enter deep sleep mode after given timeout
    if (millis() - lastActivity > SLEEP_TIMEOUT) { 
        Serial.println("Entering sleep...");
        Serial.flush();
        bleKeyboard.end();
        esp_light_sleep_start();

        // after wakeup cope with memory leak, there is no way to deinit ble device 
        // without memory leak
        // https://github.com/nkolban/esp32-snippets/issues/839
        Serial.println("Waking up...");
        bleKeyboard.begin();
        lastActivity = millis();
    }

    serverButton.read();
    
    if (bleKeyboard.isConnected()) {
        // Read current button states
        bool anyPressed = false;
        for (int i = 0; i < sizeof(keypadButtons) / sizeof(keypadButtons[0]); ++i) {
            keypadButtons[i].read();
            if (keypadButtons[i].isPressed()) {
                anyPressed = true;
            }
        }

        if (anyPressed) {
            lastActivity = millis();
        }

        batteryTask();
    }
}