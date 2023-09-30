#ifndef MIGNON_GRIND_BY_WEIGHT_START_PAGE_H
#define MIGNON_GRIND_BY_WEIGHT_START_PAGE_H

#include "../config.h"
#include "../user_interface/app.h"

struct start_page_state {
	struct config config;
	absolute_time_t save_at;
};

void start_page_init(struct page *page, struct start_page_state *state);

#endif // MIGNON_GRIND_BY_WEIGHT_START_PAGE_H
