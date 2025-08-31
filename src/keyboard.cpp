#include "keyboard.h"

static BleKeyboard bleKeyboard;

static Button serverButton = Button(PIN_SERVER_BUTTON);
static uint8_t* keypad = nullptr;

static Button keypadButtons[] = {
    Button(PIN_BUTTONS[2]),
    Button(PIN_BUTTONS[3]),
    Button(PIN_BUTTONS[0]),
    Button(PIN_BUTTONS[1])
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

void setupKeyboard() {

    // No need for max cpu performance, to save power using lower clock speed
    setCpuFrequencyMhz(80);
    bleKeyboard = BleKeyboard("Miniboard", "Pavlov sp. z o.o.", readBatteryLevel());
    bleKeyboard.begin();

    // Restart the device and boot the HTTP server
    serverButton.onPress([](){
        auto currentSettings = g_settings.get();
        currentSettings->boot_type = BOOT_HTTP_SERVER;
        g_settings.save();
        esp_restart();
    });
    serverButton.begin();

    for (int i = 0; i < sizeof(keypadButtons) / sizeof(keypadButtons[0]); ++i) {
        keypadButtons[i].begin();
        keypadButtons[i].onPress([i]() {
            dlog_v("Button %d pressed, sending keycode %d\n", i + 1, g_settings.get()->keypad[i]);
            bleKeyboard.press(g_settings.get()->keypad[i]);
        }).onRelease(releaseAll);

        // wakeup if any button is pressed
        gpio_wakeup_enable(gpio_num_t(PIN_BUTTONS[i]), GPIO_INTR_LOW_LEVEL);
    }
    
    esp_sleep_enable_gpio_wakeup();
}

void keyboardTask() {
    static int64_t lastActivity = 0;

    // Enter deep sleep mode after given timeout
    if (millis() - lastActivity > SLEEP_TIMEOUT) { 
        dlog("Entering sleep...");
        dlog_v("Memory: %d\n", esp_get_free_heap_size());
        bleKeyboard.end();
        esp_light_sleep_start();

        // after wakeup cope with memory leak, there is no way to deinit ble device 
        // without memory leak
        // https://github.com/nkolban/esp32-snippets/issues/839
        dlog("Waking up...");
        dlog_v("Memory: %d\n", esp_get_free_heap_size());
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