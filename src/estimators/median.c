#include <pico/stdlib.h>
#include <malloc.h>
#include <stdlib.h>
#include <memory.h>

struct median_estimator {
    size_t num_total_samples;
    size_t samples_buff_size;
    float samples_buff[];
};

#define num_samples(estimator) (MIN((estimator)->num_total_samples, (estimator)->samples_buff_size))
#define samples_start_idx(estimator) ((estimator)->num_total_samples > (estimator)->num_total_samples ? (estimator)->num_total_samples % (estimator)->num_total_samples : 0)

static void median_estimator_push_sample(struct median_estimator* estimator, float sample) {
    assert(estimator);

    estimator->samples_buff[estimator->num_total_samples++ % estimator->samples_buff_size] = sample;
}

int compare(const void * a, const void * b)
{
    float fa = *(const float*) a;
    float fb = *(const float*) b;
    return (fa > fb) - (fa < fb);
}

static float median_estimator_calc_median(struct median_estimator* estimator) {
    assert(estimator);
    static float buff[200] = {0};

    assert(estimator->samples_buff_size <= 200);

    memcpy(buff, estimator->samples_buff + samples_start_idx(estimator), sizeof(float) * (estimator->samples_buff_size - samples_start_idx(estimator)));
    memcpy(buff + estimator->samples_buff_size - samples_start_idx(estimator), estimator->samples_buff, sizeof(float) * (samples_start_idx(estimator)));

    qsort(buff, num_samples(estimator), sizeof(float), compare);

    size_t median_idx = num_samples(estimator) / 2;
    if (estimator->samples_buff_size % 2 == 1) {
        return buff[median_idx];
    }
    return (buff[median_idx - 1] + buff[median_idx]) / 2;
}

struct median_estimator* median_estimator_alloc(size_t samples_count) {
    struct median_estimator* estimator = NULL;

    assert(samples_count > 0);

    estimator = malloc(sizeof(estimator) + sizeof(*estimator->samples_buff) * samples_count);
    estimator->num_total_samples = 0;
    estimator->samples_buff_size = samples_count;

    return estimator;
}

void median_estimator_free(struct median_estimator* estimator) {
    assert(estimator);

    free(estimator);
}

float median_estimator_feed(struct median_estimator* estimator, float sample) {
    assert(estimator);

    median_estimator_push_sample(estimator, sample);
    return median_estimator_calc_median(estimator);
}

bool median_estimator_is_saturated(struct median_estimator* estimator) {
    assert(estimator);

    return estimator->num_total_samples >= estimator->samples_buff_size;
}
