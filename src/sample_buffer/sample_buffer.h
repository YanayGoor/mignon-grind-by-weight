#ifndef MIGNON_GRIND_BY_WEIGHT_SAMPLE_BUFFER_H
#define MIGNON_GRIND_BY_WEIGHT_SAMPLE_BUFFER_H

#include <mignon-grind-by-weight/types.h>

#include <e4c_lite.h>
#include <memory.h>
#include <stdlib.h>

E4C_DECLARE_EXCEPTION(OutOfBoundException);

struct sample_buffer {
	sample_t *buff;
	size_t buff_size;
	size_t total_sample_count;
};

void sample_buffer_init(struct sample_buffer *sample_buffer, sample_t *buff, size_t buff_size);

void sample_buffer_push(struct sample_buffer *sample_buffer, sample_t sample);

size_t sample_buffer_len(struct sample_buffer *sample_buffer);

sample_t sample_buffer_get(struct sample_buffer *sample_buffer, int idx);

void sample_buffer_copy(struct sample_buffer *sample_buffer, sample_t *dst, size_t count);

#endif // MIGNON_GRIND_BY_WEIGHT_SAMPLE_BUFFER_H
