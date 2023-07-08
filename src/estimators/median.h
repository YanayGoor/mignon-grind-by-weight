#ifndef MEDIAN_ESTIMATOR_H
#define MEDIAN_ESTIMATOR_H

#include <mignon-grind-by-weight/types.h>

#include <pico/stdlib.h>

#include "../sample_buffer/sample_buffer.h"

struct median_estimator {
	struct sample_buffer sample_buffer;
};

void median_estimator_init(struct median_estimator *, sample_t *buff, size_t buff_size);
sample_t median_estimator_feed(struct median_estimator *, sample_t sample);
bool median_estimator_is_saturated(struct median_estimator *);

#endif // MEDIAN_ESTIMATOR_H
