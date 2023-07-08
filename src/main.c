#include <pico/stdio.h>
#include <e4c_lite.h>
#include <stdio.h>

#include "estimators/linear_fitting.h"
#include "estimators/median.h"
#include "scale.h"

#define HANDLE_WEIGHT			   (354)
#define TARGET_COFFEE_WEIGHT	   (12)
#define HANDLE_THRESHOLD		   (HANDLE_WEIGHT - 15)
#define LINEAR_ESTIMATOR_BUFF_SIZE (10)

int main() {
	stdio_init_all();

	struct scale scale = {0};

	scale_init(&scale);

#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else

	while (true) {
		E4C_TRY {
			scale_zero(&scale);

			uint samples_above_threshold = 0;
			while (samples_above_threshold < SCALE_SAMPLES_PER_SECONDS / 2) {
				if (scale_read_sample(&scale).value > HANDLE_THRESHOLD) {
					samples_above_threshold++;
				} else {
					samples_above_threshold = 0;
				}
			}

			printf("handle on!\n");

			scale_zero(&scale);

			static sample_t buff[LINEAR_ESTIMATOR_BUFF_SIZE] = {0};
			struct linear_fitting_estimator estimator = {0};
			linear_fitting_estimator_init(&estimator, buff, ARRAY_SIZE(buff));
			while (linear_fitting_estimator_feed(&estimator, scale_read_sample(&scale)).value < TARGET_COFFEE_WEIGHT) {}

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

#endif
}
