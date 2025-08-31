#include "EEPROMSettings.h"

// global instance
extern EEPROMSettings g_settings = EEPROMSettings();

int EEPROMSettings::ID = 0;

void EEPROMSettings::save() {
    if (!EEPROM.begin(SIZE)) return;

    m_settings.checksum = M_checksum();
    EEPROM.put(OFFSET, m_settings);
    EEPROM.commit();
    EEPROM.end();
}

void EEPROMSettings::load() {
    if (!EEPROM.begin(SIZE)) return;
    EEPROM.get(OFFSET, m_settings);

    dlog_v("Loaded settings: {boot_type: %d, keypad: [%d, %d, %d, %d], checksum: %d}\n",
            m_settings.boot_type,
            m_settings.keypad[0],
            m_settings.keypad[1],
            m_settings.keypad[2],
            m_settings.keypad[3],
            m_settings.checksum);
    
    if (!valid()) {
        dlog("Invalid settings found, restoring defaults");
        m_settings = DEFAULT_SETTINGS;
    }
    EEPROM.end();
}

bool EEPROMSettings::valid() const {
    if (memcmp(&m_settings, &DEFAULT_SETTINGS, sizeof(eeprom_settings_t)) == 0) {
        return true; // default settings are always valid
    }

    // Check the program type
    if (m_settings.boot_type != BOOT_BLE_KEYBOARD && 
        m_settings.boot_type != BOOT_HTTP_SERVER) {
        dlog_v("Invalid boot_type: %d\n", m_settings.boot_type);
        return false;
    }

    if (M_checksum() != m_settings.checksum) {
        dlog_v("Checksum mismatch: expected %d, got %d\n", M_checksum(), m_settings.checksum);
        return false;
    }

    return true;
}


uint8_t EEPROMSettings::M_checksum() const {
    uint8_t checksum = 0;
    checksum ^= m_settings.boot_type;
    for (size_t i = 0; i < sizeof(m_settings.keypad); i++) {
        checksum ^= m_settings.keypad[i];
    }
    return checksum;
}