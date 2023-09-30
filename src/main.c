#include <mignon-grind-by-weight/defs.h>

#include <pico/multicore.h>
#include <pico/stdio.h>
#include <e4c_lite.h>
#include <sh1107_spi.h>
#include <stdio.h>

#include "config.h"
#include "estimators/linear_fitting.h"
#include "estimators/median.h"
#include "pages/final_page.h"
#include "pages/grinding_page.h"
#include "pages/start_page.h"
#include "scale.h"
#include "user_interface/app.h"
#include "user_interface/button.h"
#include "user_interface/display_sh1107.h"

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
};

void init_grinding_stage(struct grinding_stage *grinding_stage) {
	assert(read_config()->linear_estimator_sample_count <= ARRAY_SIZE(grinding_stage->samples_buff));
	linear_fitting_estimator_init(&grinding_stage->weight_estimator, grinding_stage->samples_buff,
								  read_config()->linear_estimator_sample_count);
}

bool grinding_stage_is_saturated(struct grinding_stage *grinding_stage) {
	// we don't really care that the time estimator is not saturated, we don't need the accuracy.
	return linear_fitting_estimator_is_saturated(&grinding_stage->weight_estimator);
}

sample_t grinding_stage_feed_sample(struct grinding_stage *grinding_stage, sample_t sample) {
	return linear_fitting_estimator_feed(&grinding_stage->weight_estimator, sample);
}

sample_t final_stage_get_curr_weight(struct scale *scale) {
	sample_t buff[SCALE_SAMPLES_PER_SECONDS / 4];
	struct median_estimator median_estimator = {0};
	sample_t estimate = {0};

	median_estimator_init(&median_estimator, buff, ARRAY_SIZE(buff));
	while (!median_estimator_is_saturated(&median_estimator)) {
		estimate = median_estimator_feed(&median_estimator, scale_read_sample(scale));
	}
	return estimate;
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
	sample_t estimated_weight = {0};
	static struct page start_page = {0};
	static struct page grinding_page = {0};
	static struct page final_page = {0};
	static struct start_page_state start_page_state = {0};
	static struct grinding_page_state grinding_page_state = {0};
	static struct final_page_state final_page_state = {0};
	static struct grinding_stage grinding_stage = {0};

	stdio_init_all();

	struct scale scale = {0};
	scale_init(&scale);

	sh1107_spi_init(&sh1107_spi, spi1, SCLK_PIN, MOSI_PIN, A0_PIN, CS_PIN);
	sh1107_init(&sh1107, &sh1107_hw_spi, &sh1107_spi, RES_PIN, DISPLAY_HEIGHT);

	start_page_init(&start_page, &start_page_state);
	grinding_page_init(&grinding_page, &grinding_page_state);
	final_page_init(&final_page, &final_page_state);
	app_init(&app, &display_type_sh1107, &sh1107);

	app_add_page(&app, &start_page);

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

			// NOTE: the following code assumes the target weight will never be reached before the estimators are
			//  saturated, if that is not the case for you, reduce the sample count in the estimators.
			while (!grinding_stage_is_saturated(&grinding_stage)) {
                grinding_stage_feed_sample(&grinding_stage, scale_read_sample(&scale));
			}

			while (estimated_weight.value < read_config()->target_coffee_weight) {
				estimated_weight = grinding_stage_feed_sample(&grinding_stage, scale_read_sample(&scale));
				grinding_page_update_weight(&grinding_page_state, estimated_weight);
			}

			app_add_page(&app, &final_page);
			final_page_set_weight(&final_page_state, final_stage_get_curr_weight(&scale));

			// wait for handle to be removed
			scale_zero(&scale);
			while (true) {
				scale_read_sample(&scale);
			}
		}
		E4C_CATCH(ObjectRemovedFromScaleException) {
			printf("Removed from scale, resetting\n");

			while (app.num_pages > 1) {
				app_pop_page(&app);
			}
		};
	}
}
