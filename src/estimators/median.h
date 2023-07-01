#ifndef MEDIAN_ESTIMATOR_H
#define MEDIAN_ESTIMATOR_H

#include <pico/stdlib.h>

struct median_estimator;

struct median_estimator* median_estimator_alloc(size_t samples_count);
void median_estimator_free(struct median_estimator*);
float median_estimator_feed(struct median_estimator*, float sample);
bool median_estimator_is_saturated(struct median_estimator*);

#endif //MEDIAN_ESTIMATOR_H
