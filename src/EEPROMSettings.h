#pragma once

#include <EEPROM.h>

#include "config.h"

class EEPROMSettings {   
public:
    constexpr static int OFFSET = 16, // offset in the EEPROM to store settings
                         SIZE = 512; // size of the EEPROM
    static int ID;

    EEPROMSettings() {
        ID++;
    }

    void save();
    void load();

    eeprom_settings_t* get() {
        return &m_settings;
    }

    const eeprom_settings_t* get() const {
        return &m_settings;
    }

    void set(const eeprom_settings_t& settings) {
        m_settings = settings;
    }

    bool valid() const;
private:
    uint8_t M_checksum() const;

    eeprom_settings_t m_settings { DEFAULT_SETTINGS }; // default values
};

extern EEPROMSettings g_settings;