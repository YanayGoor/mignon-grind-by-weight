#ifndef LINEAR_FITTING_ESTIMATOR_H
#define LINEAR_FITTING_ESTIMATOR_H

#include <pico/stdlib.h>

struct linear_fitting_estimator;

struct linear_fitting_estimator* linear_fitting_estimator_alloc(size_t samples_count);
void linear_fitting_estimator_free(struct linear_fitting_estimator*);
float linear_fitting_estimator_feed(struct linear_fitting_estimator*, float sample);
bool linear_fitting_estimator_is_saturated(struct linear_fitting_estimator*);

#endif //LINEAR_FITTING_ESTIMATOR_H
