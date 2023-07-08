#ifndef MIGNON_GRIND_BY_WEIGHT_TYPES_H
#define MIGNON_GRIND_BY_WEIGHT_TYPES_H

#include <pico/stdlib.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

typedef struct sample {
	float value;
	absolute_time_t time;
} sample_t;

#endif // MIGNON_GRIND_BY_WEIGHT_TYPES_H
