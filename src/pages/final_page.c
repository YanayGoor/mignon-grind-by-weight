#include "final_page.h"

#include <hardware/timer.h>
#include <pico/time.h>
#include <pico/types.h>

#include "../widgets/target_weight_text.h"
#include "mignon-grind-by-weight/defs.h"
#include "mignon-grind-by-weight/types.h"

#define PARTICLE_PADDING (0)
#define PARTICLE_SIZE	 (2)
#define PARTICLE_SPEED	 (0.0004)

#define FLASH_START_TIME_MS	 (200)
#define FLASH_STOP_TIME_MS	 (FLASH_START_TIME_MS + 100)
#define FLASH_FINISH_TIME_MS (FLASH_STOP_TIME_MS + 100)

void final_page_update(void *user_data, struct display_type *display_type, void *display) {
	struct final_page_state *state = (struct final_page_state *)user_data;

	display_type->display_fill(display, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 1);

	if (is_nil_time(state->start_time)) {
		return;
	}

	if (time_reached(delayed_by_ms(state->start_time, FLASH_START_TIME_MS))) {
		target_weight_text_draw(display_type, display, state->weight, 0);
	}
	if (time_reached(delayed_by_ms(state->start_time, FLASH_STOP_TIME_MS))) {
		target_weight_text_draw(display_type, display, state->weight, 1);
	}
	if (time_reached(delayed_by_ms(state->start_time, FLASH_FINISH_TIME_MS))) {
		target_weight_text_draw(display_type, display, state->weight, 0);
	}

	display_type->display_update(display);
}

void final_page_set_weight(struct final_page_state *state, sample_t weight) {
	state->start_time = get_absolute_time();
	state->weight = weight.value;
}

void final_page_init(struct page *page, struct final_page_state *state) {
	state->weight = 0;
	state->start_time = nil_time;
	page->on_left_click = NULL;
	page->on_left_long_click = NULL;
	page->on_left_double_click = NULL;
	page->on_right_click = NULL;
	page->on_right_long_click = NULL;
	page->on_right_double_click = NULL;
	page->update = final_page_update;
	page->user_data = state;
}
