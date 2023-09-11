#include "app.h"

#include <mignon-grind-by-weight/defs.h>

static struct page *get_active_page(struct app *app) {
	return &app->pages[app->num_pages - 1];
}

static void app_on_left_click(void *user_data) {
	struct app *app = (struct app *)user_data;
	if (app->num_pages > 0 && get_active_page(app)->on_left_click) {
		get_active_page(app)->on_left_click(get_active_page(app)->user_data);
	}
}

static void app_on_left_double_click(void *user_data) {
	struct app *app = (struct app *)user_data;
	if (app->num_pages > 0 && get_active_page(app)->on_left_double_click) {
		get_active_page(app)->on_left_double_click(get_active_page(app)->user_data);
	}
}

static void app_on_left_long_click(void *user_data) {
	struct app *app = (struct app *)user_data;
	if (app->num_pages > 0 && get_active_page(app)->on_left_long_click) {
		get_active_page(app)->on_left_long_click(get_active_page(app)->user_data);
	}
}

static void app_on_right_click(void *user_data) {
	struct app *app = (struct app *)user_data;
	if (app->num_pages > 0 && get_active_page(app)->on_right_click) {
		get_active_page(app)->on_right_click(get_active_page(app)->user_data);
	}
}

static void app_on_right_double_click(void *user_data) {
	struct app *app = (struct app *)user_data;
	if (app->num_pages > 0 && get_active_page(app)->on_right_double_click) {
		get_active_page(app)->on_right_double_click(get_active_page(app)->user_data);
	}
}

static void app_on_right_long_click(void *user_data) {
	struct app *app = (struct app *)user_data;
	if (app->num_pages > 0 && get_active_page(app)->on_right_long_click) {
		get_active_page(app)->on_right_long_click(get_active_page(app)->user_data);
	}
}

void app_init(struct app *app, struct display_hw *display_hw, void *display_hw_data) {
	button_init(&app->left_button, LEFT_BUTTON_GPIO, app_on_left_click, app_on_left_double_click,
				app_on_left_long_click, app);
	button_init(&app->right_button, RIGHT_BUTTON_GPIO, app_on_right_click, app_on_right_double_click,
				app_on_right_long_click, app);
	app->display_hw = display_hw;
	app->display_hw_data = display_hw_data;
}
void app_update(struct app *app) {
	button_update(&app->left_button);
	button_update(&app->right_button);

	if (app->num_pages > 0) {
		get_active_page(app)->update(get_active_page(app)->user_data, app->display_hw, app->display_hw_data);
	}
}

void app_add_page(struct app *app, struct page *page) {
	app->pages[app->num_pages++] = *page;
}

void app_pop_page(struct app *app) {
	app->num_pages--;
}
