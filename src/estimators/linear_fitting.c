#include "linear_fitting.h"

#include <pico/stdlib.h>

void linear_fitting_estimator_init(struct linear_fitting_estimator *estimator, sample_t *buff, size_t buff_size) {
	assert(estimator);

	sample_buffer_init(&estimator->sample_buffer, buff, buff_size);
}

static sample_t linear_fitting_estimator_calculate_estimate(struct linear_fitting_estimator *estimator) {
	float sum_x = 0;
	float sum_x_square = 0;
	float sum_y = 0;
	float sum_x_times_y = 0;
	uint n = 0;

	assert(estimator);

	n = sample_buffer_len(&estimator->sample_buffer);

	// we can't calculate a slope of a function based on one sample.
	if (n == 1) {
		return sample_buffer_get(&estimator->sample_buffer, 0);
	}

	for (int i = 0; i < n; i++) {
		sample_t sample = sample_buffer_get(&estimator->sample_buffer, i);
		sum_x += to_us_since_boot(sample.time);
		sum_x_square += to_us_since_boot(sample.time) * to_us_since_boot(sample.time);
		sum_x_times_y += to_us_since_boot(sample.time) * sample.value;
		sum_y += sample.value;
	}

	float b = (n * sum_x_times_y - sum_x * sum_y) / (n * sum_x_square - sum_x * sum_x);
	float a = (sum_y - b * sum_x) / n;

	sample_t last_sample = sample_buffer_get(&estimator->sample_buffer, -1);

	return (sample_t){.time = last_sample.time, .value = a + b * to_us_since_boot(last_sample.time)};
}

sample_t linear_fitting_estimator_feed(struct linear_fitting_estimator *estimator, sample_t sample) {
	assert(estimator);

	sample_buffer_push(&estimator->sample_buffer, sample);
	return linear_fitting_estimator_calculate_estimate(estimator);
}

bool linear_fitting_estimator_is_saturated(struct linear_fitting_estimator *estimator) {
	return estimator->sample_buffer.total_sample_count >= estimator->sample_buffer.buff_size;
}
