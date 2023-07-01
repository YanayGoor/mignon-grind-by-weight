#ifndef LINEAR_FITTING_ESTIMATOR_H
#define LINEAR_FITTING_ESTIMATOR_H

#include <pico/stdlib.h>

struct linear_fitting_estimator;

void linear_fitting_alloc(struct median_estimator**, size_t samples_count);
void linear_fitting_free(struct median_estimator*);
float linear_fitting_feed(float sample);

#endif //LINEAR_FITTING_ESTIMATOR_H
