#include "app.h"

#include <mignon-grind-by-weight/defs.h>

static void app_on_left_click(void *user_data) {
	struct app *app = (struct app *)user_data;
	if (app->curr_page.on_left_click) {
		app->curr_page.on_left_click(app->curr_page.user_data);
	}
}

static void app_on_left_double_click(void *user_data) {
	struct app *app = (struct app *)user_data;
	if (app->curr_page.on_left_double_click) {
		app->curr_page.on_left_double_click(app->curr_page.user_data);
	}
}

static void app_on_left_long_click(void *user_data) {
	struct app *app = (struct app *)user_data;
	if (app->curr_page.on_left_long_click) {
		app->curr_page.on_left_long_click(app->curr_page.user_data);
	}
}

static void app_on_right_click(void *user_data) {
	struct app *app = (struct app *)user_data;
	if (app->curr_page.on_right_click) {
		app->curr_page.on_right_click(app->curr_page.user_data);
	}
}

static void app_on_right_double_click(void *user_data) {
	struct app *app = (struct app *)user_data;
	if (app->curr_page.on_right_double_click) {
		app->curr_page.on_right_double_click(app->curr_page.user_data);
	}
}

static void app_on_right_long_click(void *user_data) {
	struct app *app = (struct app *)user_data;
	if (app->curr_page.on_right_long_click) {
		app->curr_page.on_right_long_click(app->curr_page.user_data);
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
	critical_section_enter_blocking(&app->page_lock);
	app->curr_page = app->pages[app->num_pages - 1];
	critical_section_exit(&app->page_lock);

	button_update(&app->left_button);
	button_update(&app->right_button);

	if (app->num_pages > 0) {
		app->curr_page.update(app->curr_page.user_data, app->display_hw, app->display_hw_data);
	}
}

void app_add_page(struct app *app, struct page *page) {
	critical_section_enter_blocking(&app->page_lock);
	app->pages[app->num_pages++] = *page;
	critical_section_exit(&app->page_lock);
}

void app_pop_page(struct app *app) {
	critical_section_enter_blocking(&app->page_lock);
	app->num_pages--;
	critical_section_exit(&app->page_lock);
}
