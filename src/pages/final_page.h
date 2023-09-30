#ifndef MIGNON_GRIND_BY_WEIGHT_FINAL_PAGE_H
#define MIGNON_GRIND_BY_WEIGHT_FINAL_PAGE_H

#include <pico/time.h>

#include "../config.h"
#include "../user_interface/app.h"
#include "mignon-grind-by-weight/types.h"

struct final_page_state {
	float weight;
	absolute_time_t start_time;
};

void final_page_init(struct page *page, struct final_page_state *user_data);
void final_page_set_weight(struct final_page_state *state, sample_t weight);

#endif // MIGNON_GRIND_BY_WEIGHT_FINAL_PAGE_H
