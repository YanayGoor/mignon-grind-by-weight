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
#define PARTICLE_SPEED	 (2)

struct coffee_particle {
	bool initialized;
	uint x;
	uint y;
};

struct grinding_page_state {
	struct config config;
	float prev_weight;
	float curr_weight;
	absolute_time_t finish_time;
	struct coffee_particle particles[70];
};

static struct grinding_page_state state = {0};

void grinding_page_update(void *user_data, struct display_hw *display_hw, void *display_hw_data) {
	//	display_hw->display_fill(display_hw_data, 0, 0, 128, 128, 0);
	uint prev_height = MIN(128 * state.prev_weight / state.config.target_coffee_weight, 128);
	uint height = MIN(128 * state.curr_weight / state.config.target_coffee_weight, 128);
	if (height == 128 && is_nil_time(state.finish_time)) {
		state.finish_time = get_absolute_time();
	}
	for (uint i = 0; i < sizeof(state.particles) / sizeof(state.particles[0]); i++) {
		struct coffee_particle *particle = &state.particles[i];
		if (particle->initialized) continue;
		particle->x = PARTICLE_PADDING + rand() % (128 - (PARTICLE_SIZE - 1) - 2 * PARTICLE_PADDING);
		particle->y = 0;
		particle->initialized = true;
		break;
	}
	for (uint i = 0; i < sizeof(state.particles) / sizeof(state.particles[0]); i++) {
		struct coffee_particle *particle = &state.particles[i];
		if (!particle->initialized) continue;
		particle->y += PARTICLE_SPEED;
		if (particle->y >= 128 - height) {
			particle->initialized = false;
			continue;
		}
		display_hw->display_fill(display_hw_data, particle->y - PARTICLE_SPEED, particle->x, PARTICLE_SIZE,
								 PARTICLE_SPEED, 0);
		display_hw->display_fill(display_hw_data, particle->y, particle->x, PARTICLE_SIZE, PARTICLE_SIZE, 1);
	}
	display_hw->display_fill(display_hw_data, 128 - height, 0, 128, height - prev_height, 1);

	state.prev_weight = state.curr_weight;
	state.curr_weight += 0.01;

	if (!is_nil_time(state.finish_time) && time_reached(delayed_by_ms(state.finish_time, 400))) {
		target_weight_text_draw(display_hw, display_hw_data, state.config.target_coffee_weight, 0);
	}
	if (!is_nil_time(state.finish_time) && time_reached(delayed_by_ms(state.finish_time, 500))) {
		target_weight_text_draw(display_hw, display_hw_data, state.config.target_coffee_weight, 1);
	}
	if (!is_nil_time(state.finish_time) && time_reached(delayed_by_ms(state.finish_time, 600))) {
		target_weight_text_draw(display_hw, display_hw_data, state.config.target_coffee_weight, 0);
	}
	display_hw->display_update(display_hw_data);
}

void grinding_page_init() {
	state.prev_weight = 0;
	state.curr_weight = 0;
	state.config = *read_config();
	state.finish_time = nil_time;
}

struct page grinding_page = {
	.on_left_click = NULL,
	.on_left_long_click = NULL,
	.on_left_double_click = NULL,
	.on_right_click = NULL,
	.on_right_long_click = NULL,
	.on_right_double_click = NULL,
	.update = grinding_page_update,
	.user_data = NULL,
};