#include "app.h"

#include <mignon-grind-by-weight/defs.h>

void app_init(struct app *app) {
	button_init(&app->left_button, LEFT_BUTTON_GPIO, NULL, NULL, NULL, NULL);
	button_init(&app->right_button, RIGHT_BUTTON_GPIO, NULL, NULL, NULL, NULL);
}
void app_update(struct app *app) {
	button_update(&app->left_button);
	button_update(&app->right_button);
}