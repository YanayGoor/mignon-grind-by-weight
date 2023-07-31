#ifndef MIGNON_GRIND_BY_WEIGHT_DEFS_H
#define MIGNON_GRIND_BY_WEIGHT_DEFS_H

#define LEFT_BUTTON_GPIO  (3)
#define RIGHT_BUTTON_GPIO (4)

#define SCALE_SAMPLES_PER_SECONDS (80)

#define MAX_SAMPLES_BUFF_LEN (SCALE_SAMPLES_PER_SECONDS * 5)

#define DEFAULT_HANDLE_DETECTION_SAMPLE_COUNT	  (80)
#define DEFAULT_HANDLE_WEIGHT					  (354)
#define DEFAULT_HANDLE_WEIGHT_RANGE				  (20)
#define DEFAULT_TARGET_COFFEE_WEIGHT			  (14)
#define DEFAULT_LINEAR_ESTIMATOR_SAMPLE_COUNT	  (100)
#define DEFAULT_SCALE_OBJECT_LIFTED_OFF_THRESHOLD (-30)
#define DEFAULT_SCALE_ZERO_SAMPLE_COUNT			  (SCALE_SAMPLES_PER_SECONDS / 2)
#define DEFAULT_SCALE_TO_GRAMS_MULTIPLIER		  (-0.009422787707787109)

#endif // MIGNON_GRIND_BY_WEIGHT_DEFS_H
