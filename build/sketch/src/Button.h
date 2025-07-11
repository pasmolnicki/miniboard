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
    static constexpr unsigned int debounceDelay = 40; // 50ms debounce delay
    static constexpr unsigned int holdTimeDefault = 900; // Default hold time in milliseconds (after this delay, the callback will be rapidly triggered)
    static constexpr unsigned int holdDelay = 20; // delay between rapid callbacks
    
    enum ButtonPinMode {
        MODE_INPUT_PULLUP,
        MODE_INPUT_PULLDOWN
    };

    // Press button constructor
    Button(int pin, internal::callback_t callback, ButtonPinMode mode = MODE_INPUT_PULLUP)
        : m_callback(callback) {
        M_init_pin_state(pin, mode);
    }

    // Hold button constructor with specific hold time
    Button(int pin, internal::callback_t callback, unsigned short hold_time, ButtonPinMode mode = MODE_INPUT_PULLUP)
        : m_callback(callback) {
        M_init_pin_state(pin, mode);
        m_state.rapid_hold_time = hold_time;
    }

    // Read the button state, and possibly trigger the callback
    void read();

private:
    void M_init_pin_state(int pin, ButtonPinMode mode);

    struct pin_state_t {
        unsigned int pin: 27;
        unsigned int state: 1;
        unsigned int last_state: 1;
        unsigned int pressed_state: 1;
        unsigned int released_state: 1;
        unsigned int hold_state: 1;
        unsigned int last_debounce_time;
        unsigned int current_hold_time:16;
        unsigned int rapid_hold_time:16;
        unsigned int last_hold_timer;
    } m_state;

    internal::callback_t m_callback; // Callback function for button events
};