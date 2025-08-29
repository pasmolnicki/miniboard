#include "src/BleKeyboard.h"
#include "src/ButtonLib.h"

/*

Bluetooth Keyboard

Made by IlikeChooros
License: MIT

*/

constexpr int SLEEP_TIMEOUT = 15 * 60 * 1000, // 15 minutes
              BATTERY_UPDATE_INTERVAL = 10 * 1000; // 10 seconds

constexpr int PIN_BATTERY_LEVEL = 13,
              PIN_LED_BUILTIN = 2;

// buttons 1, 2, 3, 4 -> GPIOs 26, 25, 14, 27
// which means the connection mapping looks like this:
// 3, 4, 1, 2 (the gpios are in order: 14, 27, 26, 25 on the esp32 board)
constexpr int PIN_BUTTONS[] = {
    26, 25, 14, 27
};

BleKeyboard bleKeyboard;

Button buttons[] = {
    Button(PIN_BUTTONS[0]),
    Button(PIN_BUTTONS[1]),
    Button(PIN_BUTTONS[2]),
    Button(PIN_BUTTONS[3])
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
 * Using 47 + 100 k ohm voltage divider, with li ion 3.7v battery,
 * so 0% is about 3.2v and 100% is about 4.1v, with that voltage divider
 * I get 100/147 * 3.2v = 2.176v, and 100/147 * 4.1v = 2.789v
 */
uint8_t readBatteryLevel() {
    constexpr int BATTERY_LEVEL_MIN_MV = 2176,
                  BATTERY_LEVEL_MAX_MV = 2789;

    int rawMv = analogReadMilliVolts(PIN_BATTERY_LEVEL);
    int mapped = std::max(BATTERY_LEVEL_MIN_MV, std::min(BATTERY_LEVEL_MAX_MV, rawMv));
    return uint8_t(float(mapped - BATTERY_LEVEL_MIN_MV) / float(BATTERY_LEVEL_MAX_MV - BATTERY_LEVEL_MIN_MV) * 100.0f);
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

// Setup button callbacks and wakeup protocols
void setup() {
    // battery level pin
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_BATTERY_LEVEL),
        .mode = GPIO_MODE_INPUT,
    };
    gpio_config(&io_conf);

    // turn on led on startup
    gpio_config_t led_conf = {
        .pin_bit_mask = (1ULL << PIN_LED_BUILTIN),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&led_conf);
    gpio_set_level(gpio_num_t(PIN_LED_BUILTIN), HIGH);

    bleKeyboard = BleKeyboard("Miniboard", "Pavlov sp. z o.o.", readBatteryLevel());
    bleKeyboard.begin();
    Serial.begin(115200);
    
    delay(500);
    gpio_set_level(gpio_num_t(PIN_LED_BUILTIN), LOW);

    Serial.println("Miniboard config");
    Serial.printf("XTAL: %dMhz\n", getXtalFrequencyMhz());
    Serial.printf("Low cpu freq mode: %d\n", setCpuFrequencyMhz(80));
    Serial.printf("CPU: %dMhz\n", getCpuFrequencyMhz());
    
    for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); ++i) {
        buttons[i].begin();
        buttons[i].onPress(callbacks[i]).onRelease(releaseAll);

        // wakeup if any button is pressed
        gpio_wakeup_enable(gpio_num_t(PIN_BUTTONS[i]), GPIO_INTR_LOW_LEVEL);
    }
    
    esp_sleep_enable_gpio_wakeup();
    Serial.println("BLE Keyboard started");
}

void loop() {
    static int64_t lastActivity = 0;

    // Enter deep sleep mode after given timeout
    if (millis() - lastActivity > SLEEP_TIMEOUT) { 
        Serial.println("Entering sleep...");
        Serial.flush();
        bleKeyboard.end();
        Serial.flush();
        esp_light_sleep_start();

        // after wakeup cope with memory leak, there is no way to deinit ble device 
        // without memory leak
        // https://github.com/nkolban/esp32-snippets/issues/839
        Serial.println("Waking up...");
        bleKeyboard.begin();
        lastActivity = millis();
    }

    if (bleKeyboard.isConnected()) {
        // Read current button states
        bool anyPressed = false;
        for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); ++i) {
            buttons[i].read();
            if (buttons[i].isPressed()) {
                anyPressed = true;
            }
        }

        if (anyPressed) {
            lastActivity = millis();
        }

        batteryTask();
    }
}