#include "Button.h"

// Helper function to call the callback if it is set
inline void _make_callback(internal::callback_t& callback) {
    if (callback) {
        callback();
    }
}

void Button::read() {
    int reading = digitalRead(m_state.pin);

    // Start debounce timer if the state has changed
    if (reading != m_state.last_state) {
        m_state.last_debounce_time = millis();
    }
    
    // Check for rapid hold callbacks while button is pressed
    if (reading == m_state.pressed_state && 
        m_state.last_hold_timer != 0 && 
        millis() - m_state.last_hold_timer >= m_state.current_hold_time
    ) {
        // If the hold time has been reached, trigger the callback
        _make_callback(m_on_hold);
        m_state.last_hold_timer = millis(); // Reset hold timer for next rapid fire
        m_state.current_hold_time = m_state.rapid_hold_time; // Switch to rapid fire mode
        return; // Exit early since we already handled the hold
    }
    

    // If the reading is stable for longer than the debounce delay, update the state
    if (millis() - m_state.last_debounce_time > debounceDelay) {
        if (reading != m_state.state) {
            m_state.state = reading;

            // Check for button press or hold
            if (m_state.state == m_state.pressed_state) {
                // Button is pressed
                _make_callback(m_on_press);
                // Begin hold timer
                m_state.last_hold_timer = millis();
                m_state.current_hold_time = holdTimeDefault; // Set long hold delay
            } else {
                m_state.last_hold_timer = 0; // Reset hold timer on release
                _make_callback(m_on_release);
            }
        }
    }
    
    // Always update the last state for next iteration
    m_state.last_state = reading;
}


// Initialize the pin state based on the button type and mode
void Button::M_init_pin_state(int pin, ButtonPinMode mode) {
    if (mode == MODE_INPUT_PULLUP) {
        m_state.state = HIGH; // Default state for INPUT_PULLUP
        m_state.pressed_state = LOW; // Button pressed state
        m_state.released_state = HIGH; // Button released state
        pinMode(pin, INPUT_PULLUP);
    } else {
        m_state.state = LOW; // Default state for INPUT_PULLDOWN
        m_state.pressed_state = HIGH; // Button pressed state
        m_state.released_state = LOW; // Button released state
        pinMode(pin, INPUT_PULLDOWN);
    }
    m_state.pin = pin;
    m_state.last_state = m_state.state;
    m_state.last_debounce_time = 0;
    m_state.current_hold_time = Button::holdTimeDefault; // Default hold time
    m_state.last_hold_timer = 0; // Reset hold timer
}