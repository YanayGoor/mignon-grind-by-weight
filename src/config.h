#ifndef MIGNON_GRIND_BY_WEIGHT_CONFIG_H
#define MIGNON_GRIND_BY_WEIGHT_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

struct __attribute__((__packed__)) config {
	uint32_t handle_detection_sample_count;
	uint32_t handle_weight;
	uint32_t handle_weight_range;
	float target_coffee_weight;
	uint32_t linear_estimator_sample_count;
	float scale_object_lifted_off_threshold;
	uint32_t scale_zero_sample_count;
	double scale_to_grams_multiplier;
};

const struct config *read_config();
void save_config(struct config *config);

#endif // MIGNON_GRIND_BY_WEIGHT_CONFIG_H
