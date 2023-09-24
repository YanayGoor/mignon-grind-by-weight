#include <mignon-grind-by-weight/defs.h>

#include <pico/multicore.h>
#include <pico/stdio.h>
#include <e4c_lite.h>
#include <sh1107_spi.h>
#include <stdio.h>

#include "config.h"
#include "estimators/linear_fitting.h"
#include "pages/grinding_page.h"
#include "pages/main_page.h"
#include "scale.h"
#include "user_interface/app.h"
#include "user_interface/button.h"
#include "user_interface/display_sh1107.h"

#define US_GET_SEC(val)	 ((val) / 1000000)
#define US_GET_MSEC(val) ((val)*1000 / 1000000)

#define GET_UPPER_THRESHOLD(base, range) ((base) * (100 + (range)) / 100)
#define GET_LOWER_THRESHOLD(base, range) ((base) * (100 - (range)) / 100)

#define SCLK_PIN (10)
#define MOSI_PIN (11)
#define CS_PIN	 (9)
#define A0_PIN	 (8)
#define RES_PIN	 (12)

bool is_sample_within_handle_range(sample_t sample) {
	return sample.value < GET_UPPER_THRESHOLD(read_config()->handle_weight, read_config()->handle_weight_range) &&
		   sample.value > GET_LOWER_THRESHOLD(read_config()->handle_weight, read_config()->handle_weight_range);
}

struct grinding_stage {
	sample_t samples_buff[MAX_SAMPLES_BUFF_LEN];
	struct linear_fitting_estimator weight_estimator;

	sample_t weights_buff[MAX_SAMPLES_BUFF_LEN];
	struct linear_fitting_estimator time_estimator;
};

void init_grinding_stage(struct grinding_stage *main_stage) {
	assert(read_config()->linear_estimator_sample_count <= ARRAY_SIZE(main_stage->samples_buff));
	linear_fitting_estimator_init(&main_stage->weight_estimator, main_stage->samples_buff,
								  read_config()->linear_estimator_sample_count);

	assert(read_config()->linear_estimator_sample_count <= ARRAY_SIZE(main_stage->weights_buff));
	linear_fitting_estimator_init(&main_stage->time_estimator, main_stage->weights_buff,
								  read_config()->linear_estimator_sample_count);
}

bool grinding_stage_is_saturated(struct grinding_stage *main_stage) {
	// we don't really care that the time estimator is not saturated, we don't need the accuracy.
	return linear_fitting_estimator_is_saturated(&main_stage->weight_estimator);
}

sample_t grinding_stage_feed_sample(struct grinding_stage *main_stage, sample_t sample) {
	sample_t weight_estimate = linear_fitting_estimator_feed(&main_stage->weight_estimator, sample);
	linear_fitting_estimator_feed(&main_stage->time_estimator, weight_estimate);
	return weight_estimate;
}

uint64_t grinding_stage_get_estimated_time_to_target_weight(struct grinding_stage *main_stage, uint32_t target_weight) {
	return get_estimated_time_until_value(&main_stage->time_estimator, target_weight);
}

struct sh1107_spi sh1107_spi = {0};
struct sh1107 sh1107 = {0};
struct app app = {0};

void second_core_main(void) {
	while (true) {
		app_update(&app);
	}
}

int main() {
    static struct page main_page = {0};
    static struct page grinding_page = {0};
    static struct main_page_state main_page_state = {0};
    static struct grinding_page_state grinding_page_state = {0};
    static struct grinding_stage grinding_stage = {0};

    stdio_init_all();

	struct scale scale = {0};
	scale_init(&scale);

	sh1107_spi_init(&sh1107_spi, spi1, SCLK_PIN, MOSI_PIN, A0_PIN, CS_PIN);
	sh1107_init(&sh1107, &sh1107_hw_spi, &sh1107_spi, RES_PIN, 128);

    main_page_init(&main_page, &main_page_state);
    grinding_page_init(&grinding_page, &grinding_page_state);
	app_init(&app, &sh1107_display, &sh1107);

	app_add_page(&app, &main_page);

    multicore_launch_core1(second_core_main);

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

            init_grinding_stage(&grinding_stage);

			app_add_page(&app, &grinding_page);

			// NOTE: when the grinder starts it probably starts slower than it is at the end, so the sample buffer
			//  should not the values from the start all the way to the end.
			//  due to this the following code assumes the target weight will never be reached before the estimators are
			//  saturated, if that is not the case for you, reduce the sample count in the estimators.

			// we want to make sure the estimators are saturated, otherwise they might give us bad estimates that might
			// stop the main stage prematurely.
			while (grinding_stage_is_saturated(&grinding_stage)) {
                grinding_stage_feed_sample(&grinding_stage, scale_read_sample(&scale));
			}

			sample_t estimated_weight = grinding_stage_feed_sample(&grinding_stage, scale_read_sample(&scale));
			while (estimated_weight.value < read_config()->target_coffee_weight) {
				estimated_weight = grinding_stage_feed_sample(&grinding_stage, scale_read_sample(&scale));
                grinding_page_state.curr_weight = estimated_weight.value;
				uint64_t time_left =
                        grinding_stage_get_estimated_time_to_target_weight(&grinding_stage,
                                                                           read_config()->target_coffee_weight);
				printf("time left %lld.%lld weight %f\n", US_GET_SEC(time_left), US_GET_MSEC(time_left),
					   estimated_weight.value);
			}

			printf("DONE! waiting for handle to be removed\n");

			scale_zero(&scale);

			while (true) {
				scale_read_sample(&scale);
			}
		} E4C_CATCH(ObjectRemovedFromScaleException) {
			printf("Removed from scale, resetting\n");

            if (app.num_pages > 1) {
                app_pop_page(&app);
            }
		};
	}
}
