#include "median.h"

#include <pico/stdlib.h>
#include <stdlib.h>

#define QSORT_MAX_SIZE (100)

int sample_compare(const void *a, const void *b) {
	sample_t sample_a = *(const sample_t *)a;
	sample_t sample_b = *(const sample_t *)b;
	return (sample_a.value > sample_b.value) - (sample_a.value < sample_b.value);
}

static sample_t median_estimator_calc_median(struct median_estimator *estimator) {
	static sample_t buff[QSORT_MAX_SIZE] = {0};

	assert(estimator);
	assert(estimator->sample_buffer.buff_size <= QSORT_MAX_SIZE);

	size_t sample_count = sample_buffer_len(&estimator->sample_buffer);

	sample_buffer_copy(&estimator->sample_buffer, buff, sample_count);

	qsort(buff, sample_count, sizeof(sample_t), sample_compare);

	return (sample_t){
		.time = sample_buffer_get(&estimator->sample_buffer, -1).time,
		.value = sample_count % 2 == 1 ? buff[sample_count / 2].value
									   : (buff[sample_count / 2 - 1].value + buff[sample_count / 2].value) / 2,
	};
}

void median_estimator_init(struct median_estimator *estimator, sample_t *buff, size_t buff_size) {
	assert(estimator);

	sample_buffer_init(&estimator->sample_buffer, buff, buff_size);
}

sample_t median_estimator_feed(struct median_estimator *estimator, sample_t sample) {
	assert(estimator);

	sample_buffer_push(&estimator->sample_buffer, sample);
	return median_estimator_calc_median(estimator);
}

bool median_estimator_is_saturated(struct median_estimator *estimator) {
	assert(estimator);

	return estimator->sample_buffer.total_sample_count >= estimator->sample_buffer.buff_size;
}
