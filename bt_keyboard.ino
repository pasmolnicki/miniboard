#include <BleKeyboard.h>
#include <ButtonLib.h>
#include <Keypad.h>

/**
 * Button pin definitions
 */

constexpr int BUTTON_PIN_ARROW_UP = 14,
              BUTTON_PIN_ARROW_DOWN = 27,
              BUTTON_PIN_BACKSPACE = 25,
              BUTTON_PIN_ENTER = 26,
              PIN_BATTERY_LEVEL = 13;

BleKeyboard bleKeyboard = BleKeyboard("Miniboard", "Pavlov sp. z o.o.", 50);

Button buttons[] = {
    Button(BUTTON_PIN_ARROW_UP),
    Button(BUTTON_PIN_ARROW_DOWN),
    Button(BUTTON_PIN_BACKSPACE),
    Button(BUTTON_PIN_ENTER)
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

void setup() {
    Serial.begin(115200);
    delay(500);
    bleKeyboard.begin();
    
    for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); ++i) {
        buttons[i].begin();
        buttons[i].onPress(callbacks[i]).onRelease(releaseAll);
    }

    Serial.println("BLE Keyboard started");
}

void loop(){
    if (bleKeyboard.isConnected()) {
        // Read current button states
        for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); ++i) {
            buttons[i].read();
        }
    }
    batteryTask();
}