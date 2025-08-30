#pragma once

#include <EEPROM.h>

#include "config.h"

class EEPROMSettings {   
public:
    constexpr static int OFFSET = 16, // offset in the EEPROM to store settings
                         SIZE = 512; // size of the EEPROM

    EEPROMSettings() = default;

    void save() {
        if (!EEPROM.begin(SIZE)) return;

        m_settings.checksum = M_checksum();
        EEPROM.put(OFFSET, m_settings);
        EEPROM.commit();
        EEPROM.end();
    }

    void load() {
        if (!EEPROM.begin(SIZE)) return;
        EEPROM.get(OFFSET, m_settings);
        if (!valid()) {
            m_settings = DEFAULT_SETTINGS;
        }
        EEPROM.end();
    }

    eeprom_settings_t* get() {
        return &m_settings;
    }

    const eeprom_settings_t* get() const {
        return &m_settings;
    }

    void set(const eeprom_settings_t& settings) {
        m_settings = settings;
    }

    inline bool valid() const {
        if (memcmp(&m_settings, &DEFAULT_SETTINGS, sizeof(eeprom_settings_t)) == 0) {
            return true; // default settings are always valid
        }

        // Check the program type
        if (m_settings.boot_type != BOOT_BLE_KEYBOARD && 
            m_settings.boot_type != BOOT_HTTP_SERVER) {
            return false;
        }

        if (M_checksum() != m_settings.checksum) {
            return false;
        }

        return true;
    }
private:
    uint8_t M_checksum() const {
        uint8_t checksum = 0;
        checksum ^= m_settings.boot_type;
        for (size_t i = 0; i < sizeof(m_settings.keypad); i++) {
            checksum ^= m_settings.keypad[i];
        }
        return checksum;
    }

    eeprom_settings_t m_settings { DEFAULT_SETTINGS }; // default values
};