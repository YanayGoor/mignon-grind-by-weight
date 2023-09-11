#include "button.h"

#include <hardware/gpio.h>
#include <pico/printf.h>
#include <pico/time.h>

#define CLICK_MIN_US				  (50000)
#define LONG_CLICK_MIN_US			  (400000)
#define LONG_CLICK_COOLDOWN_US		  (100000)
#define DOUBLE_CLICK_COOLDOWN_US	  (300000)
#define DOUBLE_CLICK_MIN_US			  (CLICK_MIN_US)
#define DOUBLE_CLICK_UNPRESSED_MIN_US (40000)
#define DOUBLE_CLICK_UNPRESSED_MAX_US (120000)

static void button_clear_state(struct button *button) {
	button->state.is_pressed = false;
	button->state.prev_is_pressed = false;
	button->state.time_pressed = 0;
	button->state.time_unpressed = 0;
	button->state.prev_time_pressed = 0;
	button->state.prev_time_unpressed = 0;
	button->state.long_press_registered = false;
	button->state.cooldown_until = nil_time;
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

	if (!is_nil_time(button->state.cooldown_until) && !time_reached(button->state.cooldown_until)) {
		return;
	}

	if (button->state.prev_time_pressed > DOUBLE_CLICK_MIN_US &&
		button->state.prev_time_unpressed > DOUBLE_CLICK_UNPRESSED_MIN_US &&
		button->state.prev_time_unpressed < DOUBLE_CLICK_UNPRESSED_MAX_US &&
		button->state.time_pressed > DOUBLE_CLICK_MIN_US && !pressed) {
		if (button->on_double_click != NULL) button->on_double_click(button->user_data);
		// we don't want 3 clicks to register as two double clicks.
		button_clear_state(button);
		button->state.cooldown_until = delayed_by_us(get_absolute_time(), DOUBLE_CLICK_COOLDOWN_US);
	} else if (button->state.time_pressed > LONG_CLICK_MIN_US && !button->state.long_press_registered) {
		button->state.long_press_registered = true;
		button->state.cooldown_until = delayed_by_us(get_absolute_time(), LONG_CLICK_COOLDOWN_US);
		if (button->on_double_click != NULL) button->on_long_click(button->user_data);
	} else if (button->state.time_pressed > CLICK_MIN_US && !button->state.long_press_registered && !pressed) {
		if (button->on_click != NULL) button->on_click(button->user_data);
	} else if (button->state.long_press_registered && !pressed) {
		button->state.long_press_registered = false;
	}

	button_update_state(button, pressed);
}
