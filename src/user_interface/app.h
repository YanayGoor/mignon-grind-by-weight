#ifndef MIGNON_GRIND_BY_WEIGHT_APP_H
#define MIGNON_GRIND_BY_WEIGHT_APP_H

#include "button.h"

struct app {
	struct button left_button;
	struct button right_button;
};

void app_init(struct app *);
void app_update(struct app *);

#endif // MIGNON_GRIND_BY_WEIGHT_APP_H
