#include <pico/stdlib.h>
#include <malloc.h>

struct median_estimator {
    size_t samples_count;
    float samples_buff[];
};

struct median_estimator* linear_fitting_alloc(size_t samples_count) {
    struct median_estimator* estimator = NULL;

    assert(samples_count > 0);

    estimator = malloc(sizeof(estimator) + sizeof(*estimator->samples_buff) * samples_count);
    estimator->samples_count = samples_count;

    return estimator;
}
void linear_fitting_free(struct median_estimator*);
float linear_fitting_feed(float sample);
