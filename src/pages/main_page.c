#include "main_page.h"

#include <hardware/timer.h>
#include <pico/printf.h>
#include <pico/time.h>

#include "../config.h"

struct config config = {0};
absolute_time_t save_at;

void app_on_left_click(void *user_data) {
	config.target_coffee_weight -= 0.1;
}

void app_on_left_double_click(void *user_data) {
	config.target_coffee_weight -= 0.9;
}

void app_on_left_long_click(void *user_data) {}

void app_on_right_click(void *user_data) {
	config.target_coffee_weight += 0.1;
}

void app_on_right_double_click(void *user_data) {
	config.target_coffee_weight += 0.9;
}

void app_on_right_long_click(void *user_data) {}

void main_page_update(void *user_data) {
	if (!is_nil_time(save_at) && time_reached(save_at)) {
		save_config(&config);
	}
	printf("target %f\n", config.target_coffee_weight);
}

void main_page_init() {
	save_at = nil_time;
	config = *read_config();
}

struct page main_page = {
	.on_left_click = app_on_left_click,
	.on_left_double_click = app_on_left_double_click,
	.on_left_long_click = app_on_left_long_click,
	.on_right_click = app_on_right_click,
	.on_right_double_click = app_on_right_double_click,
	.on_right_long_click = app_on_right_long_click,
	.update = main_page_update,
	.user_data = NULL,
};
