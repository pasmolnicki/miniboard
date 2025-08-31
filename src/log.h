#pragma once

#include <HardwareSerial.h>

#define LOG_TAG "Miniboard"

// #define dlog(msg) Serial.println(msg)
#define dlog(msg) (void)msg;

// #define dlog_v(msg, ...) Serial.printf("[" LOG_TAG "]: " msg, ##__VA_ARGS__)
#define dlog_v(msg, ...) (void)msg;

// #define dlog_t(tag, msg, ...) Serial.printf("[" tag "]: " msg, ##__VA_ARGS__)
#define dlog_t(tag, msg, ...) (void)msg;