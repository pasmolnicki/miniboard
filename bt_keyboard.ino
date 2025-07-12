#include <BleKeyboard.h>
#include "src/Button.h"

BleKeyboard bleKeyboard;

constexpr int BUTTON_PIN_1 = 25,
              BUTTON_PIN_2 = 26;

Button enter = Button(BUTTON_PIN_1);
Button right_arrow = Button(BUTTON_PIN_2);

void releaseAll() {
    bleKeyboard.releaseAll();
}

template <uint8_t key>
void press() {
    bleKeyboard.press(key);
}

void setup() {
    Serial.begin(115200);
    delay(500);
    bleKeyboard.setName("Wiktor Keyboard");
    bleKeyboard.begin();
    Serial.println("BLE Keyboard started");

    enter.onPress(press<KEY_RETURN>).onRelease(releaseAll);
    right_arrow.onPress(press<KEY_RIGHT_ARROW>).onRelease(releaseAll);
}

void loop(){
    if (bleKeyboard.isConnected()) {
        // Read current button states
        enter.read();
        right_arrow.read();
    }
}