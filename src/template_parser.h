#pragma once

#include <vector>
#include <string>
#include <cstring>


typedef struct {
    const char* name;
    std::string value;
} parser_value_t;

typedef std::vector<parser_value_t> parser_args_t;

void parse(const char* temp, char* result, parser_args_t);