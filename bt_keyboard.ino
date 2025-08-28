#include <BleKeyboard.h>
#include <ButtonLib.h>
#include <Keypad.h>

/*

Bluetooth Keyboard

License: MIT

*/

constexpr int PIN_BATTERY_LEVEL = 13;

constexpr int PIN_BUTTONS[] = {
    14, 27, 25, 26
};

BleKeyboard bleKeyboard = BleKeyboard("Miniboard", "Pavlov sp. z o.o.", 50);

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
    press<KEY_UP_ARROW>,
    press<KEY_DOWN_ARROW>,
    press<KEY_BACKSPACE>,
    press<KEY_RETURN>
};

void releaseAll() {
    bleKeyboard.releaseAll();
}

/**
 * Read the battery level and set the bt characteristic.
 * Using 47 + 100 k ohm voltage divider, with li ion 3.7v battery,
 * so 0% is about 3.4v and 100% is about 4.2v, with that voltage divider
 * I get 100/147 * 3.4v = 2.313v, and 100/147 * 4.2v = 2.857v
 */
void batteryTask() {
    constexpr int BATTERY_LEVEL_MIN_MV = 2313,
                  BATTERY_LEVEL_MAX_MV = 2857;

    static int64_t lastUpdate = 0;

    if (millis() - lastUpdate > 10000) {
        lastUpdate = millis();
        int rawMv = analogReadMilliVolts(PIN_BATTERY_LEVEL);
        int mapped = std::max(BATTERY_LEVEL_MIN_MV, std::min(BATTERY_LEVEL_MAX_MV, rawMv));
        int level = int(float(mapped - BATTERY_LEVEL_MIN_MV) / float(BATTERY_LEVEL_MAX_MV - BATTERY_LEVEL_MIN_MV) * 100.0f);

        bleKeyboard.setBatteryLevel(level);
    }
}

// Setup button callbacks and wakeup protocols
void setup() {
    bleKeyboard.begin();
    Serial.begin(115200);
    delay(500);

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

void loop(){
    static int64_t lastActivity = 0;

    // Enter deep sleep mode after 5 minutes of inactivity
    if (millis() - lastActivity > 5 * 60 * 1000) {
        esp_deep_sleep_start();
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