#ifndef MIGNON_GRIND_BY_WEIGHT_BUTTON_H
#define MIGNON_GRIND_BY_WEIGHT_BUTTON_H

#include <pico/types.h>

typedef void(button_callback_t)(void *user_data);

struct button {
	struct {
		absolute_time_t updated_at;

		bool is_pressed;
		bool prev_is_pressed;
		uint64_t time_pressed;
		uint64_t time_unpressed;
		uint64_t prev_time_pressed;
		uint64_t prev_time_unpressed;
		uint64_t cooldown;
	} state;

	uint gpio_idx;
	button_callback_t *on_click;
	button_callback_t *on_double_click;
	button_callback_t *on_long_click;
	void *user_data;
};

void button_init(struct button *, uint gpio_idx, button_callback_t *on_click, button_callback_t *on_double_click,
				 button_callback_t *on_long_click, void *user_data);
void button_update(struct button *);

#endif // MIGNON_GRIND_BY_WEIGHT_BUTTON_H
