#ifndef MIGNON_GRIND_BY_WEIGHT_DISPLAY_H
#define MIGNON_GRIND_BY_WEIGHT_DISPLAY_H

#include <pico/types.h>
#include <font.h>

typedef void(display_text_t)(void *user_data, const char *text, uint row, uint col, uint color, uint size, font_t *font,
							 enum text_alignment alignment);
typedef void(display_update_t)(void *user_data);

struct display_hw {
	display_text_t *display_text;
	display_update_t *display_update;
};

#endif // MIGNON_GRIND_BY_WEIGHT_DISPLAY_H
