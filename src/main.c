/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <exceptions4c/lite/src/e4c_lite.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <stdio.h>

#define try E4C_TRY
#define catch E4C_CATCH
#define finally E4C_FINALLY
#define throw E4C_THROW

int main() {
    stdio_init_all();

#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        try {
            throw(NullPointerException, "null pointer yo!\n");
        } catch(NullPointerException) {
            printf("null pointer caught\n");
        }
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_PIN, 0);
        sleep_ms(400);
    }
#pragma clang diagnostic pop
#endif
}