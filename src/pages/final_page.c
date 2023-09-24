#include "grinding_page.h"

#include <hardware/timer.h>
#include <pico/printf.h>
#include <pico/time.h>
#include <pico/types.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../config.h"
#include "../widgets/target_weight_text.h"
#include "fonts/product_sans_bold.h"

#define PARTICLE_PADDING (0)
#define PARTICLE_SIZE	 (2)
#define PARTICLE_SPEED	 (0.0004)

#define FLASH_START_TIME_MS	 (400)
#define FLASH_STOP_TIME_MS	 (500)
#define FLASH_FINISH_TIME_MS (600)

#define FOREACH_PARTICLE(particle, particles)                                                                          \
	for (particle = &(particles)[0]; particle < &(particles)[0] + sizeof(particles) / sizeof((particles)[0]);          \
		 particle++)

void grinding_page_update(void *user_data, struct display_hw *display_hw, void *display_hw_data) {
    struct grinding_page_state *state = (struct grinding_page_state *)user_data;
	struct coffee_particle *particle = NULL;

	uint prev_height = MIN(128 * state->prev_weight / state->config.target_coffee_weight, 128);
	uint height = MIN(128 * state->curr_weight / state->config.target_coffee_weight, 128);

	if (time_reached(delayed_by_ms(state->last_particle_created_at, 5))) {
		FOREACH_PARTICLE(particle, state->particles) {
			if (particle->initialized) continue;
			particle->x = PARTICLE_PADDING + rand() % (128 - (PARTICLE_SIZE - 1) - 2 * PARTICLE_PADDING);
			particle->y = 0;
			particle->initialized = true;
			particle->last_updated = get_absolute_time();
			break;
		}
		state->last_particle_created_at = get_absolute_time();
	}

	FOREACH_PARTICLE(particle, state->particles) {
		if (!particle->initialized) continue;

		int64_t time_since_last_update = absolute_time_diff_us(particle->last_updated, get_absolute_time());
		float last_y = particle->y;

		particle->y += PARTICLE_SPEED * time_since_last_update;
		particle->last_updated = get_absolute_time();

		if ((uint)particle->y + PARTICLE_SIZE >= 128 - height) {
			particle->initialized = false;
			continue;
		}

		display_hw->display_fill(display_hw_data, (uint)last_y, particle->x, PARTICLE_SIZE,
								 (uint)particle->y - (uint)last_y, 0);
		display_hw->display_fill(display_hw_data, (uint)particle->y, particle->x, PARTICLE_SIZE, PARTICLE_SIZE, 1);
	}

	display_hw->display_fill(display_hw_data, 128 - height, 0, 128, height - prev_height, 1);

	state->prev_weight = state->curr_weight;
	state->curr_weight += 0.01;

	if (height == 128 && is_nil_time(state->fin_time)) {
		state->fin_time = get_absolute_time();
	}

	if (!is_nil_time(state->fin_time) && time_reached(delayed_by_ms(state->fin_time, FLASH_START_TIME_MS))) {
		target_weight_text_draw(display_hw, display_hw_data, state->config.target_coffee_weight, 0);
	}
	if (!is_nil_time(state->fin_time) && time_reached(delayed_by_ms(state->fin_time, FLASH_STOP_TIME_MS))) {
		target_weight_text_draw(display_hw, display_hw_data, state->config.target_coffee_weight, 1);
	}
	if (!is_nil_time(state->fin_time) && time_reached(delayed_by_ms(state->fin_time, FLASH_FINISH_TIME_MS))) {
		target_weight_text_draw(display_hw, display_hw_data, state->config.target_coffee_weight, 0);
	}
	display_hw->display_update(display_hw_data);
}

void grinding_page_init(struct page *page, struct grinding_page_state *state) {
	state->prev_weight = 0;
	state->curr_weight = 0;
	state->config = *read_config();
	state->fin_time = nil_time;
    page->on_left_click = NULL;
    page->on_left_long_click = NULL;
    page->on_left_double_click = NULL;
    page->on_right_click = NULL;
    page->on_right_long_click = NULL;
    page->on_right_double_click = NULL;
    page->update = grinding_page_update;
    page->user_data = NULL;
}
