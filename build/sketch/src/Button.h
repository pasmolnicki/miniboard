#line 1 "/home/minis/Desktop/bt_keyboard/src/Button.h"
#pragma once

#include <Arduino.h>

namespace internal {
    #if defined(ESP32) || defined(ESP8266)
        // If that's an ESP32 or ESP8266, we can use the built-in functions

        #include <functional>
        // for callbacks, otherwise we need to define our own
        typedef std::function<void()> callback_t;

    #else 
        // For other platforms, we define a simple callback type
        typedef void (*callback_t)();
    #endif
}

class Button {
public:
    static constexpr unsigned int debounceDelay = 50.0; // 50ms debounce delay

    enum ButtonType {
        PRESS_TYPE,
        HOLD_TYPE
    };

    enum ButtonPinMode {
        MODE_INPUT_PULLUP,
        MODE_INPUT_PULLDOWN
    };

    // Press button constructor
    Button(int pin, internal::callback_t callback, ButtonPinMode mode = MODE_INPUT_PULLUP)
        : m_type(PRESS_TYPE), m_callback(callback) {
        M_init_pin_state(pin, PRESS_TYPE, mode);
    }

    // Hold button constructor with specific hold time
    Button(int pin, internal::callback_t callback, int hold_time, ButtonPinMode mode = MODE_INPUT_PULLUP)
        : m_type(HOLD_TYPE), m_callback(callback) {
        M_init_pin_state(pin, HOLD_TYPE, mode);
        m_state.hold_time = hold_time;
    }

    // Read the button state, and possibly trigger the callback
    void read();

private:
    void M_init_pin_state(int pin, ButtonType type, ButtonPinMode mode);

    struct pin_state_t {
        unsigned int pin: 28;
        unsigned int state: 1;
        unsigned int last_state: 1;
        unsigned int pressed_state: 1;
        unsigned int released_state: 1;
        unsigned int last_debounce_time;
        unsigned int hold_time;
        unsigned int last_hold_timer;
    } m_state;

    ButtonType m_type;
    internal::callback_t m_callback; // Callback function for button events
};