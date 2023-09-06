#include "display_sh1107.h"

#include <sh1107.h>

#include "display.h"

struct display_hw sh1107_display = {
	.display_text = &sh1107_text,
	.display_update = &sh1107_show,
};
