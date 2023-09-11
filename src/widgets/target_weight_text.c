#include <stdio.h>

#include "../user_interface/display.h"
#include "fonts/product_sans_bold.h"

#define WIDGET_MAX_LEN	 (100)
#define SCREEN_SIZE		 (128)
#define WIDGET_FONT_SIZE (40)
#define WIDGET_ROW		 (SCREEN_SIZE / 2 - 28)
#define WIDGET_COL		 (SCREEN_SIZE / 2)

void target_weight_text_draw(struct display_hw *display_hw, void *display_hw_data, float target_weight, bool color) {
	static char buff[WIDGET_MAX_LEN];
	snprintf(buff, WIDGET_MAX_LEN, "%.1fg", target_weight);
	display_hw->display_text(display_hw_data, buff, WIDGET_ROW, WIDGET_COL, color, WIDGET_FONT_SIZE,
							 &font_product_sans_bold, text_align_center);
}