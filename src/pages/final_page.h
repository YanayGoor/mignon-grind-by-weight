#ifndef MIGNON_GRIND_BY_WEIGHT_GRINDING_PAGE_H
#define MIGNON_GRIND_BY_WEIGHT_GRINDING_PAGE_H

#include "../config.h"
#include "../user_interface/app.h"
#include <pico/time.h>

struct final_page_state {
    float curr_weight;
    absolute_time_t start_time;
};

void final_page_init(struct page *page, struct final_page_state *user_data);

#endif // MIGNON_GRIND_BY_WEIGHT_GRINDING_PAGE_H
