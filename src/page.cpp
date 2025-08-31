#include "page.h"

static char* page = nullptr;

void initPage() {
    page = (char*)malloc(sizeof(PAGE_HTML) + 64);
    if (page) {
        parse(PAGE_HTML, page);
    }
}

char* getPage() {
    return page;
}