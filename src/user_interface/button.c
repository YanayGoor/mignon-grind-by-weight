#include "button.h"

#include <hardware/gpio.h>
#include <pico/printf.h>
#include <pico/time.h>

#define CLICK_MIN_UPDATES				   (30000)
#define LONG_CLICK_MIN_UPDATES			   (120000)
#define DOUBLE_CLICK_COOLDOWN_UPDATES	   (50000)
#define DOUBLE_CLICK_MIN_UPDATES		   (30)
#define DOUBLE_CLICK_UNPRESSED_MIN_UPDATES (3000)
#define DOUBLE_CLICK_UNPRESSED_MAX_UPDATES (60000)

static void button_clear_state(struct button *button) {
	button->state.is_pressed = false;
	button->state.prev_is_pressed = false;
	button->state.time_pressed = 0;
	button->state.time_unpressed = 0;
	button->state.prev_time_pressed = 0;
	button->state.prev_time_unpressed = 0;
	button->state.cooldown = 0;
	button->state.updated_at = get_absolute_time();
}

void button_init(struct button *button, uint gpio_idx, button_callback_t *on_click, button_callback_t *on_double_click,
				 button_callback_t *on_long_click, void *user_data) {
	gpio_init(gpio_idx);
	gpio_set_dir(gpio_idx, GPIO_IN);
	gpio_pull_up(gpio_idx);

	button->gpio_idx = gpio_idx;
	button->on_click = on_click;
	button->on_double_click = on_double_click;
	button->on_long_click = on_long_click;
	button->user_data = user_data;
	button_clear_state(button);
}

static void button_update_state(struct button *button, bool pressed) {
	absolute_time_t now = get_absolute_time();
	uint64_t us_since_last_update = absolute_time_diff_us(button->state.updated_at, now);

	if (button->state.is_pressed && pressed) {
		button->state.time_pressed += us_since_last_update;
	} else if (!button->state.is_pressed && !pressed) {
		button->state.time_unpressed += us_since_last_update;
	} else if (button->state.is_pressed && !pressed) {
		button->state.prev_time_pressed = button->state.time_pressed;
		button->state.time_pressed = 0;
	} else if (!button->state.is_pressed && pressed) {
		button->state.prev_time_unpressed = button->state.time_unpressed;
		button->state.time_unpressed = 0;
	}
	button->state.prev_is_pressed = button->state.is_pressed;
	button->state.is_pressed = pressed;
	button->state.updated_at = now;
}

void button_update(struct button *button) {
	bool pressed = !gpio_get(button->gpio_idx);

	if (button->state.cooldown > 0) {
		button->state.cooldown--;
		return;
	}

	if (button->state.prev_time_pressed > DOUBLE_CLICK_MIN_UPDATES &&
		button->state.prev_time_unpressed > DOUBLE_CLICK_UNPRESSED_MIN_UPDATES &&
		button->state.prev_time_unpressed < DOUBLE_CLICK_UNPRESSED_MAX_UPDATES &&
		button->state.time_pressed > DOUBLE_CLICK_MIN_UPDATES && !pressed) {
		button->on_double_click(button->user_data);
		// we don't want 3 clicks to register as two double clicks.
		button_clear_state(button);
		button->state.cooldown = DOUBLE_CLICK_COOLDOWN_UPDATES;
	} else if (button->state.time_pressed > LONG_CLICK_MIN_UPDATES && !pressed) {
		button->on_long_click(button->user_data);
	} else if (button->state.time_pressed > CLICK_MIN_UPDATES && !pressed) {
		button->on_click(button->user_data);
	}

	button_update_state(button, pressed);
}
