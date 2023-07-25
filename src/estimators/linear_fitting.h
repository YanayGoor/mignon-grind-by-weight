#ifndef LINEAR_FITTING_ESTIMATOR_H
#define LINEAR_FITTING_ESTIMATOR_H

#include <mignon-grind-by-weight/types.h>

#include <pico/stdlib.h>

#include "../sample_buffer/sample_buffer.h"

struct linear_fitting_estimator {
	struct sample_buffer sample_buffer;

	struct {
		float a;
		float b;
	} params;
};

void linear_fitting_estimator_init(struct linear_fitting_estimator *, sample_t *buff, size_t buff_size);
sample_t linear_fitting_estimator_feed(struct linear_fitting_estimator *, sample_t sample);
bool linear_fitting_estimator_is_saturated(struct linear_fitting_estimator *);
uint64_t get_estimated_time_until_value(struct linear_fitting_estimator *estimator, float value);

#endif // LINEAR_FITTING_ESTIMATOR_H
