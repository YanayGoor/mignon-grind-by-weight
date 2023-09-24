#ifndef MIGNON_GRIND_BY_WEIGHT_MAIN_PAGE_H
#define MIGNON_GRIND_BY_WEIGHT_MAIN_PAGE_H

#include "../user_interface/app.h"
#include "../config.h"

struct main_page_state {
    struct config config;
    absolute_time_t save_at;
};

void main_page_init(struct page *page, struct main_page_state *state);

#endif // MIGNON_GRIND_BY_WEIGHT_MAIN_PAGE_H
