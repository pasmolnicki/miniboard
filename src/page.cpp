#include "page.h"

static char* page = nullptr;

void initPage() {
    page = (char*)malloc(sizeof(PAGE_HTML) + 64);
    if (page) {
        parser_args_t args = {
            {"k0", std::to_string(DEFAULT_SETTINGS.keypad[0])},
            {"k1", std::to_string(DEFAULT_SETTINGS.keypad[1])},
            {"k2", std::to_string(DEFAULT_SETTINGS.keypad[2])},
            {"k3", std::to_string(DEFAULT_SETTINGS.keypad[3])},
        };
        parse(PAGE_HTML, page, args);
    }
}

char* getPage() {
    return page;
}