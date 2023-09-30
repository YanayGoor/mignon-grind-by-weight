#include "display_sh1107.h"

#include <sh1107.h>

#include "display.h"

struct display_type display_type_sh1107 = {
	.display_fill = &sh1107_fill,
	.display_text = &sh1107_text,
	.display_update = &sh1107_show,
};
