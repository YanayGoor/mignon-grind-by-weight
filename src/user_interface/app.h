#ifndef MIGNON_GRIND_BY_WEIGHT_APP_H
#define MIGNON_GRIND_BY_WEIGHT_APP_H

#include "button.h"

#define MAX_PAGES (5)

typedef void(update_callback_t)(void *user_data);

struct page {
	button_callback_t *on_left_click;
	button_callback_t *on_left_double_click;
	button_callback_t *on_left_long_click;
	button_callback_t *on_right_click;
	button_callback_t *on_right_double_click;
	button_callback_t *on_right_long_click;
	update_callback_t *update;
	void *user_data;
};

struct app {
	struct button left_button;
	struct button right_button;
	size_t num_pages;
	struct page pages[MAX_PAGES];
};

void app_init(struct app *);
void app_update(struct app *);
void app_add_page(struct app *, struct page *);
void app_pop_page(struct app *);

#endif // MIGNON_GRIND_BY_WEIGHT_APP_H
