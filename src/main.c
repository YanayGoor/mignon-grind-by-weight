#include <pico/stdio.h>
#include <e4c_lite.h>
#include <stdio.h>

#include "estimators/linear_fitting.h"
#include "estimators/median.h"
#include "scale.h"

#define HANDLE_WEIGHT		 (354)
#define TARGET_COFFEE_WEIGHT (12)
#define HANDLE_THRESHOLD	 (HANDLE_WEIGHT / 2)

#define LINEAR_ESTIMATOR_BUFF_SIZE (100)

#define US_GET_SEC(val)	 ((val) / 1000000)
#define US_GET_MSEC(val) ((val)*1000 / 1000000)

int main() {
	stdio_init_all();

	struct scale scale = {0};
	scale_init(&scale);

	while (true) {
		E4C_TRY {
			scale_zero(&scale);

			uint samples_above_threshold = 0;
			while (samples_above_threshold < SCALE_SAMPLES_PER_SECONDS) {
				if (scale_read_sample(&scale).value > HANDLE_THRESHOLD) {
					samples_above_threshold++;
				} else {
					samples_above_threshold = 0;
				}
			}

			printf("handle on!\n");

			scale_zero(&scale);

			static sample_t samples_buff[LINEAR_ESTIMATOR_BUFF_SIZE] = {0};
			static sample_t weights_buff[LINEAR_ESTIMATOR_BUFF_SIZE] = {0};
			struct linear_fitting_estimator weight_estimator = {0};
			struct linear_fitting_estimator time_estimator = {0};
			linear_fitting_estimator_init(&weight_estimator, samples_buff, ARRAY_SIZE(samples_buff));
			linear_fitting_estimator_init(&time_estimator, weights_buff, ARRAY_SIZE(weights_buff));
			sample_t weight = {0};
			while (!linear_fitting_estimator_is_saturated(&time_estimator) || weight.value < TARGET_COFFEE_WEIGHT) {
				weight = linear_fitting_estimator_feed(&weight_estimator, scale_read_sample(&scale));

				linear_fitting_estimator_feed(&time_estimator, weight);
				uint64_t time_left = get_estimated_time_until_value(&time_estimator, TARGET_COFFEE_WEIGHT);
				printf("time left %lld.%lld weight %f\n", US_GET_SEC(time_left), US_GET_MSEC(time_left), weight.value);
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
