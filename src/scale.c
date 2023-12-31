#include "scale.h"

#include <mignon-grind-by-weight/defs.h>

#include <pico/stdlib.h>
#include <common.h>
#include <e4c_lite.h>
#include <hx711.h>
#include <stdio.h>

#include "config.h"
#include "estimators/linear_fitting.h"
#include "estimators/median.h"

#ifndef HX711_CLK_PIN
#define HX711_CLK_PIN (14)
#endif

#ifndef HX711_DT_PIN
#define HX711_DT_PIN (15)
#endif

E4C_DEFINE_EXCEPTION(ObjectRemovedFromScaleException, "object removed from scale", RuntimeException);

static sample_t scale_read_raw_sample(struct scale *scale) {
	return (sample_t){.value = hx711_get_value(&scale->hx), .time = get_absolute_time()};
}

void scale_zero(struct scale *scale) {
	static sample_t buff[MAX_SAMPLES_BUFF_LEN] = {0};
	struct median_estimator median_estimator = {0};

	assert(read_config()->scale_zero_sample_count <= ARRAY_SIZE(buff));
	median_estimator_init(&median_estimator, buff, read_config()->scale_zero_sample_count);

	while (!median_estimator_is_saturated(&median_estimator)) {
		scale->base = median_estimator_feed(&median_estimator, scale_read_raw_sample(scale)).value;
	}
}

sample_t scale_read_sample(struct scale *scale) {
	sample_t raw_sample = scale_read_raw_sample(scale);
	sample_t sample = {.value = (raw_sample.value - scale->base) * scale->multiplier, .time = raw_sample.time};
	printf("sample %fg\n", sample.value);
	if (sample.value < read_config()->scale_object_lifted_off_threshold) {
		E4C_THROW(ObjectRemovedFromScaleException, "object removed from scale after zeroing");
	}
	return sample;
}

void scale_init(struct scale *scale) {
	hx711_get_default_config(&scale->hxcfg);

	scale->hxcfg.clock_pin = HX711_CLK_PIN;
	scale->hxcfg.data_pin = HX711_DT_PIN;

	hx711_init(&scale->hx, &scale->hxcfg);
	hx711_power_up(&scale->hx, hx711_gain_128);

	// optional
	hx711_set_gain(&scale->hx, hx711_gain_128);
	hx711_power_down(&scale->hx);
	hx711_wait_power_down();
	hx711_power_up(&scale->hx, hx711_gain_128);

	hx711_wait_settle(hx711_rate_80);

	scale_zero(scale);

	scale->multiplier = read_config()->scale_to_grams_multiplier;
}
