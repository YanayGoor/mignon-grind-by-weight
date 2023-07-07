/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *   
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <e4c_lite.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <stdio.h>
#include <hx711.h>
#include <common.h>

#include "estimators/linear_fitting.h"
#include "estimators/median.h"

#define HANDLE_WEIGHT (354)
#define TARGET_COFFEE_WEIGHT (12)
#define HANDLE_THRESHOLD (HANDLE_WEIGHT - 15)
#define SCALE_MIN_THRESHOLD (-15)

E4C_DEFINE_EXCEPTION(ObjectRemovedFromScaleException, "object removed from scale", RuntimeException);

struct scale {
    hx711_t hx;
    hx711_config_t hxcfg;
    float base;
    float multiplier;
};


void scale_zero(struct scale* scale) {
    static float buff[100] = {0};
    struct median_estimator median_estimator = {0};
    median_estimator_init(&median_estimator, buff, 100);
    while (!median_estimator_is_saturated(&median_estimator)) {
        scale->base = median_estimator_feed(&median_estimator, hx711_get_value(&scale->hx));
    }
}

float scale_read_sample(struct scale* scale) {
    float sample = (hx711_get_value(&scale->hx) - scale->base) * scale->multiplier;
    printf("sample %fg\n", sample);
    if (sample < SCALE_MIN_THRESHOLD) {
        E4C_THROW(ObjectRemovedFromScaleException, "");
    }
    return sample;
}

void scale_init(struct scale* scale) {
    hx711_get_default_config(&scale->hxcfg);

    scale->hxcfg.clock_pin = 14;
    scale->hxcfg.data_pin = 15;

    hx711_init(&scale->hx, &scale->hxcfg);
    hx711_power_up(&scale->hx, hx711_gain_128);

    // optional
    hx711_set_gain(&scale->hx, hx711_gain_128);
    hx711_power_down(&scale->hx);
    hx711_wait_power_down();
    hx711_power_up(&scale->hx, hx711_gain_128);

    hx711_wait_settle(hx711_rate_80);

    scale_zero(scale);

    scale->multiplier = -0.009422787707787109;
}

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
            while (samples_above_threshold < 50) {
                if (scale_read_sample(&scale) > HANDLE_THRESHOLD) {
                    samples_above_threshold++;
                } else {
                    samples_above_threshold = 0;
                }
            }

            printf("handle on!\n");

            scale_zero(&scale);


            static float buff[10] = {0};
            struct linear_fitting_estimator estimator = {0};
            linear_fitting_estimator_init(&estimator, buff, 10);
            while (linear_fitting_estimator_feed(&estimator, scale_read_sample(&scale)) < TARGET_COFFEE_WEIGHT) {
            }

            printf("DONE! waiting for handle to be removed\n");

            scale_zero(&scale);

            while (true) {
                scale_read_sample(&scale);
            }

        } E4C_CATCH(ObjectRemovedFromScaleException) {
            printf("Removed from scale, resetting\n");
        };
    }

#endif
}
