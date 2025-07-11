#include "Button.h"


void Button::read() {
    
    // If the button is of HOLD_TYPE, we need to check if it has been held long enough
    if (m_type == HOLD_TYPE && m_state.state == m_state.pressed_state) {
        if (m_state.last_hold_timer != 0 && millis() - m_state.last_hold_timer >= m_state.hold_time) {
            // If the hold time has been reached, trigger the callback
            m_callback();
            return; // Exit early since we already handled the hold
        }
    }

    int reading = digitalRead(m_state.pin);

    // Start debounce timer if the state has changed
    if (reading != m_state.last_state) {
        m_state.last_debounce_time = millis();
    }

    // If the reading is stable for longer than the debounce delay, update the state
    if (millis() - m_state.last_debounce_time > debounceDelay) {
        if (reading != m_state.state) {
            m_state.state = reading;

            // Check for button press or hold
            if (m_state.state == m_state.pressed_state) {
                // Button is pressed
                if (m_type == PRESS_TYPE) {
                    m_callback(); // Trigger callback for press
                }
                // If it's a hold type button, we can start the hold timer
                else if (m_type == HOLD_TYPE && m_state.hold_time > 0) {
                    m_state.last_hold_timer = millis();
                }
            } else {
                m_state.last_hold_timer = 0; // Reset hold timer on release
            }
        }
    }
    
    // Always update the last state for next iteration
    m_state.last_state = reading;
}


// Initialize the pin state based on the button type and mode
void Button::M_init_pin_state(int pin, ButtonType type, ButtonPinMode mode) {
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
    m_state.hold_time = 0; // Default hold time
    m_state.last_hold_timer = 0; // Reset hold timer
    m_type = type;
}