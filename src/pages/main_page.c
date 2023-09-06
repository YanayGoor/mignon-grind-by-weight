#include "main_page.h"

#include <hardware/timer.h>
#include <pico/printf.h>
#include <pico/time.h>

#include "../config.h"
#include "fonts/arial.h"

#define MAIN_PAGE_SAVE_AFTER_MS (5000)
#define MAIN_PAGE_SMALL_STEP	(0.1)
#define MAIN_PAGE_BIG_STEP		(1 - MAIN_PAGE_SMALL_STEP)

struct main_page_state {
	struct config config;
	absolute_time_t save_at;
};

struct main_page_state state = {0};

void app_on_left_click(void *user_data) {
	state.config.target_coffee_weight -= MAIN_PAGE_SMALL_STEP;
	state.save_at = delayed_by_ms(get_absolute_time(), MAIN_PAGE_SAVE_AFTER_MS);
}

void app_on_left_double_click(void *user_data) {
	state.config.target_coffee_weight -= MAIN_PAGE_BIG_STEP;
	state.save_at = delayed_by_ms(get_absolute_time(), MAIN_PAGE_SAVE_AFTER_MS);
}

void app_on_right_click(void *user_data) {
	state.config.target_coffee_weight += MAIN_PAGE_SMALL_STEP;
	state.save_at = delayed_by_ms(get_absolute_time(), MAIN_PAGE_SAVE_AFTER_MS);
}

void app_on_right_double_click(void *user_data) {
	state.config.target_coffee_weight += MAIN_PAGE_BIG_STEP;
	state.save_at = delayed_by_ms(get_absolute_time(), MAIN_PAGE_SAVE_AFTER_MS);
}

void main_page_update(void *user_data, struct display_hw *display_hw, void *display_hw_data) {
	if (!is_nil_time(state.save_at) && time_reached(state.save_at)) {
		save_config(&state.config);
		state.save_at = nil_time;
	}
	char buff[100];
	sprintf(buff, "%.1fg", state.config.target_coffee_weight);
	display_hw->display_text(display_hw_data, buff, 64 - 16, 64, 1, 32, &font_arial, text_align_center);
	display_hw->display_update(display_hw_data);
}

void main_page_init() {
	state.save_at = nil_time;
	state.config = *read_config();
}

struct page main_page = {
	.on_left_click = app_on_left_click,
	.on_left_double_click = app_on_left_double_click,
	.on_left_long_click = NULL,
	.on_right_click = app_on_right_click,
	.on_right_double_click = app_on_right_double_click,
	.on_right_long_click = NULL,
	.update = main_page_update,
	.user_data = NULL,
};
