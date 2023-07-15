#ifndef MIGNON_GRIND_BY_WEIGHT_SCALE_H
#define MIGNON_GRIND_BY_WEIGHT_SCALE_H

#include <mignon-grind-by-weight/types.h>

#include <e4c_lite.h>
#include <hx711.h>

E4C_DECLARE_EXCEPTION(ObjectRemovedFromScaleException);

struct scale {
	hx711_t hx;
	hx711_config_t hxcfg;
	float base;
	float multiplier;
};

void scale_zero(struct scale *scale);
sample_t scale_read_sample(struct scale *scale);
void scale_init(struct scale *scale);

#endif // MIGNON_GRIND_BY_WEIGHT_SCALE_H
