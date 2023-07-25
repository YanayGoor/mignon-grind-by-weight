#include <mignon-grind-by-weight/defs.h>

#include <pico/stdio.h>
#include <e4c_lite.h>
#include <stdio.h>

#include "config.h"
#include "estimators/linear_fitting.h"
#include "estimators/median.h"
#include "scale.h"
#include "user_interface/button.h"

#define US_GET_SEC(val)	 ((val) / 1000000)
#define US_GET_MSEC(val) ((val)*1000 / 1000000)

#define GET_UPPER_THRESHOLD(base, range) ((base) * (100 + (range)) / 100)
#define GET_LOWER_THRESHOLD(base, range) ((base) * (100 - (range)) / 100)

bool is_sample_within_handle_range(sample_t sample) {
	return sample.value < GET_UPPER_THRESHOLD(read_config()->handle_weight, read_config()->handle_weight_range) &&
		   sample.value > GET_LOWER_THRESHOLD(read_config()->handle_weight, read_config()->handle_weight_range);
}

struct main_stage {
	sample_t samples_buff[MAX_SAMPLES_BUFF_LEN];
	struct linear_fitting_estimator weight_estimator;

	sample_t weights_buff[MAX_SAMPLES_BUFF_LEN];
	struct linear_fitting_estimator time_estimator;
};

void init_main_stage(struct main_stage *main_stage) {
	assert(read_config()->linear_estimator_sample_count <= ARRAY_SIZE(main_stage->samples_buff));
	linear_fitting_estimator_init(&main_stage->weight_estimator, main_stage->samples_buff,
								  read_config()->linear_estimator_sample_count);

	assert(read_config()->linear_estimator_sample_count <= ARRAY_SIZE(main_stage->weights_buff));
	linear_fitting_estimator_init(&main_stage->time_estimator, main_stage->weights_buff,
								  read_config()->linear_estimator_sample_count);
}

bool main_stage_is_saturated(struct main_stage *main_stage) {
	// we don't really care that the time estimator is not saturated, we don't need the accuracy.
	return linear_fitting_estimator_is_saturated(&main_stage->weight_estimator);
}

sample_t main_stage_feed_sample(struct main_stage *main_stage, sample_t sample) {
	sample_t weight_estimate = linear_fitting_estimator_feed(&main_stage->weight_estimator, sample);
	linear_fitting_estimator_feed(&main_stage->time_estimator, weight_estimate);
	return weight_estimate;
}

uint64_t main_stage_get_estimated_time_to_target_weight(struct main_stage *main_stage, uint32_t target_weight) {
	return get_estimated_time_until_value(&main_stage->time_estimator, target_weight);
}

#define BUTTON_GPIO (3)

void on_button_pressed(void *bla) {
	printf("button %d pressed!\n", *(uint *)bla);
}
void on_button_long_pressed(void *bla) {
	printf("button %d long pressed!\n", *(uint *)bla);
}

void on_button_double_pressed(void *bla) {
	printf("button %d double pressed!\n", *(uint *)bla);
}

int main() {
	stdio_init_all();

	struct scale scale = {0};
	scale_init(&scale);

	struct button button = {0};
	uint button_idx = 1;
	button_init(&button, BUTTON_GPIO, on_button_pressed, on_button_double_pressed, on_button_long_pressed, &button_idx);
	struct button button2 = {0};
	uint button_idx2 = 2;
	button_init(&button2, BUTTON_GPIO + 1, on_button_pressed, on_button_double_pressed, on_button_long_pressed,
				&button_idx2);

	while (true) {
		button_update(&button);
		button_update(&button2);
	}

	while (true) {
		E4C_TRY {
			scale_zero(&scale);

			uint samples_within_range = 0;
			while (samples_within_range < read_config()->handle_detection_sample_count) {
				if (is_sample_within_handle_range(scale_read_sample(&scale))) {
					samples_within_range++;
				} else {
					samples_within_range = 0;
				}
			}

			printf("handle on!\n");

			scale_zero(&scale);

			static struct main_stage main_stage = {0};
			init_main_stage(&main_stage);

			// NOTE: when the grinder starts it probably starts slower than it is at the end, so the sample buffer
			//  should not the values from the start all the way to the end.
			//  due to this the following code assumes the target weight will never be reached before the estimators are
			//  saturated, if that is not the case for you, reduce the sample count in the estimators.

			// we want to make sure the estimators are saturated, otherwise they might give us bad estimates that might
			// stop the main stage prematurely.
			while (main_stage_is_saturated(&main_stage)) {
				main_stage_feed_sample(&main_stage, scale_read_sample(&scale));
			}

			sample_t estimated_weight = main_stage_feed_sample(&main_stage, scale_read_sample(&scale));
			while (estimated_weight.value < read_config()->target_coffee_weight) {
				estimated_weight = main_stage_feed_sample(&main_stage, scale_read_sample(&scale));
				uint64_t time_left =
					main_stage_get_estimated_time_to_target_weight(&main_stage, read_config()->target_coffee_weight);
				printf("time left %lld.%lld weight %f\n", US_GET_SEC(time_left), US_GET_MSEC(time_left),
					   estimated_weight.value);
			}

			printf("DONE! waiting for handle to be removed\n");

			scale_zero(&scale);

			while (true) {
				scale_read_sample(&scale);
			}
		}
		E4C_CATCH(ObjectRemovedFromScaleException) {
			printf("Removed from scale, resetting\n");
		};
	}
}
