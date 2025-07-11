#include <Arduino.h>
#line 1 "/home/minis/Desktop/bt_keyboard/bt_keyboard.ino"
#include <BleKeyboard.h>
#include "src/Button.h"

BleKeyboard bleKeyboard;

constexpr int BUTTON_PIN_1 = 25,
              BUTTON_PIN_2 = 26;

Button button1(BUTTON_PIN_1, [](){
    Serial.println("Button 1 pressed - Sending ENTER");
    bleKeyboard.press(KEY_RETURN); // Send Enter key
    delay(100); // Brief delay
    bleKeyboard.releaseAll(); // Release all keys
});

Button button2(BUTTON_PIN_2, [](){
    Serial.println("Button 2 pressed - Sending RIGHT ARROW");
    bleKeyboard.press(KEY_RIGHT_ARROW); // Send Right Arrow key
    delay(100); // Brief delay
    bleKeyboard.releaseAll(); // Release all keys
});

#line 23 "/home/minis/Desktop/bt_keyboard/bt_keyboard.ino"
void setup();
#line 31 "/home/minis/Desktop/bt_keyboard/bt_keyboard.ino"
void loop();
#line 23 "/home/minis/Desktop/bt_keyboard/bt_keyboard.ino"
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Hello world!");
    bleKeyboard.begin();
    Serial.println("BLE Keyboard started");
}

void loop(){
    if (bleKeyboard.isConnected()) {
        // Read current button states
        button1.read();
        button2.read();
    }
}
