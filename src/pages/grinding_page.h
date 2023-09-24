#ifndef MIGNON_GRIND_BY_WEIGHT_GRINDING_PAGE_H
#define MIGNON_GRIND_BY_WEIGHT_GRINDING_PAGE_H

#include "../config.h"
#include "../user_interface/app.h"
#include <pico/time.h>

#define PARTICLE_COUNT (70)

struct coffee_particle {
    bool initialized;
    uint x;
    float y;
    absolute_time_t last_updated;
};

struct grinding_page_state {
    struct config config;
    float prev_weight;
    float curr_weight;
    struct coffee_particle particles[PARTICLE_COUNT];
    absolute_time_t fin_time;
    absolute_time_t last_particle_created_at;
};

void grinding_page_init(struct page *page, struct grinding_page_state *user_data);

#endif // MIGNON_GRIND_BY_WEIGHT_GRINDING_PAGE_H
