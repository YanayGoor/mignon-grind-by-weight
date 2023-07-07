#ifndef MEDIAN_ESTIMATOR_H
#define MEDIAN_ESTIMATOR_H

#include <pico/stdlib.h>

struct median_estimator {
    size_t num_total_samples;
    size_t samples_buff_size;
    float* samples_buff;
};

void median_estimator_init(struct median_estimator*, float* buff, size_t buff_size);
float median_estimator_feed(struct median_estimator*, float sample);
bool median_estimator_is_saturated(struct median_estimator*);

#endif //MEDIAN_ESTIMATOR_H
