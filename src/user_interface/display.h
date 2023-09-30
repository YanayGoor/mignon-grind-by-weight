#ifndef MIGNON_GRIND_BY_WEIGHT_DISPLAY_H
#define MIGNON_GRIND_BY_WEIGHT_DISPLAY_H

#include <pico/types.h>
#include <font.h>

typedef void(display_text_t)(void *user_data, const char *text, uint row, uint col, uint color, uint size, font_t *font,
							 enum text_alignment alignment);
typedef void(display_update_t)(void *user_data);
typedef void(display_fill_t)(void *user_data, uint row, uint col, uint width, uint height, bool value);

struct display_type {
	display_fill_t *display_fill;
	display_text_t *display_text;
	display_update_t *display_update;
};

#endif // MIGNON_GRIND_BY_WEIGHT_DISPLAY_H
