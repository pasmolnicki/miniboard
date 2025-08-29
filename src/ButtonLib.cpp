#include "ButtonLib.h"

// Helper function to call the callback if it is set
inline void _make_callback(internal::callback_t& callback) {
    if (callback) {
        callback();
    }
}

void Button::read() {
    int reading = digitalRead(m_state.pin);
    unsigned int ms = millis();

    // Start debounce timer if the state has changed
    if (reading != m_state.last_state) {
        m_state.last_debounce_timer = ms;
    }
    
    // Check for rapid hold callbacks while button is pressed
    if (reading == m_state.pressed_state && 
        m_state.last_hold_timer != 0
    ) {
        // Check if there is overflow in the hold time
        if (ms < m_state.last_hold_timer) {
            // Reset hold timer if overflow occurred
            m_state.last_hold_timer = ms;
        } else if (ms - m_state.last_hold_timer >= m_state.current_hold_time) {
            // If the hold time has been reached, trigger the callback
            _make_callback(m_on_hold);
            m_state.last_hold_timer = ms; // Reset hold timer for next rapid fire
            m_state.current_hold_time = holdRapidDelay; // Switch to rapid fire mode
            return; // Exit early since we already handled the hold
        }
    }
    
    // Reset last debounce timer to avoid overflow issues
    if (ms < m_state.last_debounce_timer) {
        m_state.last_debounce_timer = ms;
    }

    // If the reading is stable for longer than the debounce delay, update the state
    if (ms - m_state.last_debounce_timer > debounceDelay) {
        if (reading != m_state.state) {
            m_state.state = reading;

            // Check for button press or hold
            if (reading == m_state.pressed_state) {
                // Button is pressed
                _make_callback(m_on_press);
                // Begin hold timer
                m_state.last_hold_timer = ms;
                m_state.begin_hold_timer = m_state.last_hold_timer;
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

void Button::begin() {
    // Set the pin mode based on the specified mode
    if (m_state.mode == MODE_INPUT_PULLUP) {
        m_state.pressed_state = LOW;
        pinMode(m_state.pin, INPUT_PULLUP);
    } else {
        m_state.pressed_state = HIGH;
        pinMode(m_state.pin, INPUT);
    }

    // Initialize the pin state and timers
    m_state.state = digitalRead(m_state.pin);
    m_state.last_state = m_state.state;
    m_state.last_debounce_timer = 0;
    m_state.current_hold_time = holdTimeDefault; // Default hold time
    m_state.last_hold_timer = 0; // Reset hold timer
    m_state.begin_hold_timer = 0; // Reset hold start timer
}
