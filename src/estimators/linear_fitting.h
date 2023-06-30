#ifndef LINEAR_FITTING_ESTIMATOR_H
#define LINEAR_FITTING_ESTIMATOR_H

#include <pico/stdlib.h>

struct linear_fitting_estimator {
    size_t num_total_samples;
    size_t samples_buff_size;
    float* samples_buff;
};

void linear_fitting_estimator_init(struct linear_fitting_estimator*, float* buff, size_t buff_size);
float linear_fitting_estimator_feed(struct linear_fitting_estimator*, float sample);
bool linear_fitting_estimator_is_saturated(struct linear_fitting_estimator*);

#endif //LINEAR_FITTING_ESTIMATOR_H
