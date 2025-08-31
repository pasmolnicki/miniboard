#pragma once

#include <HardwareSerial.h>

#define USE_LOGGING 1

#if USE_LOGGING
    #define LOG_TAG "Miniboard"
    #define dlog(msg) Serial.println("[" LOG_TAG "]: " msg)
    #define dlog_v(msg, ...) Serial.printf("[" LOG_TAG "]: " msg, ##__VA_ARGS__)
    #define dlog_t(tag, msg, ...) Serial.printf("[" tag "]: " msg, ##__VA_ARGS__)
#else
    #define dlog(msg) (void)msg;
    #define dlog_v(msg, ...) (void)msg;
    #define dlog_t(tag, msg, ...) (void)msg;
#endif