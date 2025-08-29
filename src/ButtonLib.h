#pragma once

/*

A memory-efficient button library made for Arduino IDE

Author: IlikeChooros
License: MIT

*/

#include <Arduino.h>
#include <math.h>

#define DEBOUNCE_DELAY 50 // 50ms debounce delay
#define HOLD_TIME_DEFAULT 900 // Default hold time in milliseconds (after this delay, the callback will be rapidly triggered)
#define HOLD_RAPID_DELAY 20 // delay between rapid callbacks

namespace internal {
    #if defined(ESP32) || defined(ESP8266)
        // If that's an ESP32 or ESP8266, we can use the built-in functions

        #include <functional>
        // for callbacks, otherwise we need to define our own
        typedef std::function<void()> callback_t;

    #else 
        // For other platforms, use function pointer
        typedef void (*callback_t)();
    #endif
}

// Main ButtonLib class
class Button {
public:
    static constexpr unsigned int debounceDelay = DEBOUNCE_DELAY;
    static constexpr unsigned int holdTimeDefault = HOLD_TIME_DEFAULT;
    static constexpr unsigned int holdRapidDelay = HOLD_RAPID_DELAY;
    
    // For the future, I might add more modes, but for now I'll use just two:
    enum ButtonPinMode {
        MODE_INPUT_PULLUP, // Will use INPUT_PULLUP mode, when setting the pin mode
        MODE_INPUT // Will use INPUT mode, when setting the pin mode
    };

    Button() = default; // Default constructor, initializes the button with no pin

    // Hold button constructor
    Button(int pin, ButtonPinMode mode = MODE_INPUT_PULLUP)
    {
        m_state.pin = pin;
        m_state.mode = int(mode);
    }

    Button(const Button&) = delete; // Disable copy constructor
    Button& operator=(const Button&) = delete; // Disable copy assignment

    Button(Button&&) = default; // Enable move constructor
    Button& operator=(Button&&) = default; // Enable move assignment

    // Initialize the button, setting the pin mode and initial state
    void begin();

    // Read the button state, and possibly trigger the callback
    void read();
    
    // Check if the button is currently pressed
    bool isPressed() const {
        return m_state.state == m_state.pressed_state;
    }

    // Check if the button was pressed for a certain amount of time
    // Returns true if the button was pressed for at least 'ms' milliseconds
    bool heldFor(unsigned int ms) const {
        return (m_state.last_hold_timer != 0) && 
               (millis() - m_state.begin_hold_timer >= ms);
    }

    // Get the elapsed time since the button was pressed. Not exactly the hold time, as it may return 
    // non-zero value even if the button currently is not pressed, you may want to also use isPressed() method
    unsigned int holdTime() const {
        if (m_state.begin_hold_timer == 0) return 0;
        return millis() - m_state.begin_hold_timer;
    }

    // Set the on press callback
    Button& onPress(internal::callback_t callback) {
        m_on_press = callback;
        return *this;
    }

    // Set the on release callback, also called at the end of a hold
    Button& onRelease(internal::callback_t callback) {
        m_on_release = callback;
        return *this;
    }

    // Set the on hold callback, called repeatedly while the button is held
    Button& onHold(internal::callback_t callback) {
        m_on_hold = callback;
        return *this;
    }

private:

    // Total size of the state structure is 128bits = 16 bytes
    struct pin_state_t {
        unsigned int pin: 10; // Pin number, max 1023
        unsigned int mode: 1; // Pin mode (0 for INPUT_PULLUP, 1 for INPUT)
        unsigned int state: 1; // Current state of the button (pressed or released)
        unsigned int last_state: 1; // Last state of the button (pressed or released)
        unsigned int pressed_state: 1; // State that is considered pressed (LOW for INPUT_PULLUP, HIGH for INPUT)
        unsigned int current_hold_time: 18; // Current hold time, used for rapid callbacks
        // Timers
        unsigned int begin_hold_timer; // Time point when the button was pressed
        unsigned int last_debounce_timer; // Last time the state was changed (for debouncing)
        unsigned int last_hold_timer; // Last time the hold callback was triggered
    } m_state;

    internal::callback_t m_on_press{nullptr}, 
                          m_on_release{nullptr},
                          m_on_hold{nullptr};
};
