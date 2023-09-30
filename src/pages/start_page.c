#include "start_page.h"

#include <hardware/timer.h>
#include <pico/printf.h>
#include <pico/time.h>

#include "../widgets/target_weight_text.h"
#include "mignon-grind-by-weight/defs.h"

#define START_PAGE_SAVE_AFTER_MS (5000)
#define START_PAGE_SMALL_STEP	 (0.1)
#define START_PAGE_BIG_STEP		 (1)

void app_on_left_click(void *user_data) {
	struct start_page_state *state = (struct start_page_state *)user_data;

	state->config.target_coffee_weight -= START_PAGE_SMALL_STEP;
	state->save_at = delayed_by_ms(get_absolute_time(), START_PAGE_SAVE_AFTER_MS);
}

void app_on_left_long_click(void *user_data) {
	struct start_page_state *state = (struct start_page_state *)user_data;

	state->config.target_coffee_weight -= START_PAGE_BIG_STEP;
	state->save_at = delayed_by_ms(get_absolute_time(), START_PAGE_SAVE_AFTER_MS);
}

void app_on_right_click(void *user_data) {
	struct start_page_state *state = (struct start_page_state *)user_data;

	state->config.target_coffee_weight += START_PAGE_SMALL_STEP;
	state->save_at = delayed_by_ms(get_absolute_time(), START_PAGE_SAVE_AFTER_MS);
}

void app_on_right_long_click(void *user_data) {
	struct start_page_state *state = (struct start_page_state *)user_data;

	state->config.target_coffee_weight += START_PAGE_BIG_STEP;
	state->save_at = delayed_by_ms(get_absolute_time(), START_PAGE_SAVE_AFTER_MS);
}

void start_page_update(void *user_data, struct display_type *display_type, void *display) {
	struct start_page_state *state = (struct start_page_state *)user_data;

	if (!is_nil_time(state->save_at) && time_reached(state->save_at)) {
		save_config(&state->config);
		state->save_at = nil_time;
	}
	display_type->display_fill(display, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0);
	target_weight_text_draw(display_type, display, state->config.target_coffee_weight, 1);
	display_type->display_update(display);
}

void start_page_init(struct page *page, struct start_page_state *state) {
	state->save_at = nil_time;
	state->config = *read_config();

	page->on_left_click = app_on_left_click;
	page->on_left_long_click = app_on_left_long_click;
	page->on_left_double_click = NULL;
	page->on_right_click = app_on_right_click;
	page->on_right_long_click = app_on_right_long_click;
	page->on_right_double_click = NULL;
	page->update = start_page_update;
	page->user_data = state;
}
