#include <BleKeyboard.h>
#include <ButtonLib.h>
#include <Keypad.h>

BleKeyboard bleKeyboard;

constexpr int BUTTON_PIN_ARROW_UP = 14,
              BUTTON_PIN_ARROW_DOWN = 27,
              BUTTON_PIN_BACKSPACE = 25,
              BUTTON_PIN_ENTER = 26;

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

void setup() {
    Serial.begin(115200);
    delay(500);
    bleKeyboard.setName("Miniboard");
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
}