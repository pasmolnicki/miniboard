#pragma once

enum boot_type_t {
    BOOT_BLE_KEYBOARD,
    BOOT_HTTP_SERVER
};

typedef struct {
    boot_type_t boot_type; // what program to run on boot
    uint8_t keypad[4]; // assigned keys 1-4
    uint8_t checksum; // simple checksum, xor of all bytes in struct except this one
} eeprom_settings_t;

typedef struct config_t : public eeprom_settings_t {
    IPAddress ap_ip; // IP address of the AP when in server mode

    config_t& operator=(const eeprom_settings_t& settings) {
        this->boot_type = settings.boot_type;
        memcpy(this->keypad, settings.keypad, sizeof(this->keypad));
        this->checksum = settings.checksum;
        return *this;
    }
} config_t;
