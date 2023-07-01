/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <e4c.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <stdio.h>
#include <hx711.h>
#include <common.h>

#include "estimators/median.h"


int main() {
    e4c_context_begin(E4C_TRUE);
    stdio_init_all();

    hx711_t hx = {0};
    hx711_config_t hxcfg = {0};
    hx711_get_default_config(&hxcfg);

    hxcfg.clock_pin = 14;
    hxcfg.data_pin = 15;

    hx711_init(&hx, &hxcfg);
    hx711_power_up(&hx, hx711_gain_128);

    // optional
    hx711_set_gain(&hx, hx711_gain_128);
    hx711_power_down(&hx);
    hx711_wait_power_down();
    hx711_power_up(&hx, hx711_gain_128);

    hx711_wait_settle(hx711_rate_80);

#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    bool is_tared = false;
    float base_value = 0;

    struct median_estimator* median_estimator = median_estimator_alloc(200);

    while (true) {
        float sample = hx711_get_value(&hx);
        float median = median_estimator_feed(median_estimator, sample);

        if (!is_tared && median_estimator_is_saturated(median_estimator)) {
            base_value = median;
            is_tared = true;
        }

        if (is_tared) {
            printf("%f\n", -(median - base_value));
        }
    }
#endif
    e4c_context_end();
}