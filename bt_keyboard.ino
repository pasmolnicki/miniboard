#include <BleKeyboard.h>
#include "src/Button.h"

BleKeyboard bleKeyboard;

constexpr int BUTTON_PIN_1 = 25,
              BUTTON_PIN_2 = 26;

Button enter = Button(BUTTON_PIN_1, [](){
    // Serial.println("Button 1 pressed - Sending ENTER");
    bleKeyboard.press(KEY_RETURN); // Send Enter key
    delay(80); // Brief delay
    bleKeyboard.releaseAll(); // Release all keys
});

Button right_arrow = Button(BUTTON_PIN_2, [](){
    // Serial.println("Button 2 pressed - Sending RIGHT ARROW");
    bleKeyboard.press(KEY_RIGHT_ARROW); // Send Right Arrow key
    delay(80); // Brief delay
    bleKeyboard.releaseAll(); // Release all keys
});

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("Hello world!");
    bleKeyboard.begin();
    Serial.println("BLE Keyboard started");
}

void loop(){
    if (bleKeyboard.isConnected()) {
        // Read current button states
        enter.read();
        right_arrow.read();
    }
}