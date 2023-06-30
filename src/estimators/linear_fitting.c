#include <pico/stdlib.h>
#include <malloc.h>
#include "linear_fitting.h"

void linear_fitting_estimator_init(struct linear_fitting_estimator* estimator, float* buff, size_t buff_size) {
    estimator->num_total_samples = 0;
    estimator->samples_buff_size = buff_size;
    estimator->samples_buff = buff;
}

static void linear_fitting_estimator_push_sample(struct linear_fitting_estimator* estimator, float sample) {
    assert(estimator);

    estimator->samples_buff[estimator->num_total_samples++ % estimator->samples_buff_size] = sample;
}

static float linear_fitting_estimator_calculate_estimate(struct linear_fitting_estimator* estimator) {
    float sum_x = 0;
    float sum_x_square = 0;
    float sum_y = 0;
    float sum_x_times_y = 0;
    uint n = 0;

    assert(estimator);
    assert(estimator->num_total_samples > 0);

    // we can't calculate a slope of a function based on one sample.
    if (estimator->num_total_samples == 1) {
        return estimator->samples_buff[0];
    }

    n = MIN(estimator->num_total_samples, estimator->samples_buff_size);

    for (int i = 0; i < n; i ++) {
        float sample = estimator->samples_buff[(estimator->num_total_samples - n + i) % estimator->samples_buff_size];
        sum_x += i;
        sum_x_square += i * i;
        sum_x_times_y += i * sample;
        sum_y += sample;
    }

    float b = (n * sum_x_times_y - sum_x * sum_y) / (n * sum_x_square - sum_x * sum_x);
    float a = (sum_y - b * sum_x) / n;

    return a + b * n;
}

float linear_fitting_estimator_feed(struct linear_fitting_estimator* estimator, float sample) {
    assert(estimator);

    linear_fitting_estimator_push_sample(estimator, sample);
    return linear_fitting_estimator_calculate_estimate(estimator);
}

bool linear_fitting_estimator_is_saturated(struct linear_fitting_estimator* estimator) {
    return estimator->num_total_samples >= estimator->samples_buff_size;
}

