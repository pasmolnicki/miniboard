#pragma once

#include <vector>
#include <string>
#include <cstring>

#include "log.h"

#define DEBUG_TEMPLATE 0

#if DEBUG_TEMPLATE
    #define log_temp(msg, ...) dlog_t("template", msg, ##__VA_ARGS__)
    #define log_temp_v(msg, ...) dlog_t("template", msg, ##__VA_ARGS__)
#else
    #define log_temp(msg, ...) (void)msg;
    #define log_temp_v(msg, ...) (void)msg;
#endif

typedef struct {
    const char* name;
    std::string value;
} parser_value_t;

typedef std::vector<parser_value_t> parser_args_t;

void parse(const char* temp, char* result, parser_args_t args = {});