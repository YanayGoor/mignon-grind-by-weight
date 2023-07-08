#include "sample_buffer.h"

#include <mignon-grind-by-weight/types.h>

#include <e4c_lite.h>
#include <memory.h>

E4C_DEFINE_EXCEPTION(OutOfBoundException, "index out of bounds", RuntimeException);

#define SBUFF_START_IDX(sample_buff)                                                                                   \
	(((sample_buff)->total_sample_count > (sample_buff)->buff_size)                                                    \
		 ? (sample_buff)->total_sample_count % (sample_buff)->buff_size                                                \
		 : 0)

// negative indexes start from -1 so no need to add -1 in end_idx.
#define SBUFF_END_IDX(sample_buff) ((sample_buffer)->total_sample_count % (sample_buffer)->buff_size)

#define SBUFF_LEN_UNTIL_WRAP(sample_buff) (MIN((sample_buffer)->buff_size - SBUFF_START_IDX(sample_buff), count))

void sample_buffer_init(struct sample_buffer *sample_buffer, sample_t *buff, size_t buff_size) {
	sample_buffer->buff = buff;
	sample_buffer->buff_size = buff_size;
	sample_buffer->total_sample_count = 0;
}

void sample_buffer_push(struct sample_buffer *sample_buffer, sample_t sample) {
	assert(sample_buffer);

	sample_buffer->buff[sample_buffer->total_sample_count++ % sample_buffer->buff_size] = sample;
}

sample_t sample_buffer_get(struct sample_buffer *sample_buffer, int idx) {
	assert(sample_buffer);

	if (idx >= MIN((int)sample_buffer->total_sample_count, (int)sample_buffer->buff_size)) {
		E4C_THROW(OutOfBoundException, "index out of bounds");
	}

	if (idx < -MIN((int)sample_buffer->total_sample_count, (int)sample_buffer->buff_size)) {
		E4C_THROW(OutOfBoundException, "index out of bounds");
	}

	if (idx >= 0) {
		return sample_buffer->buff[(SBUFF_START_IDX(sample_buffer) + idx) % sample_buffer->buff_size];
	}
	return sample_buffer->buff[(SBUFF_END_IDX(sample_buffer) + idx) % sample_buffer->buff_size];
}

void sample_buffer_copy(struct sample_buffer *sample_buffer, sample_t *dst, size_t count) {
	assert(sample_buffer);

	if (count > MIN(sample_buffer->total_sample_count, sample_buffer->buff_size)) {
		E4C_THROW(OutOfBoundException, "index out of bounds");
	}

	memcpy(dst, sample_buffer->buff + SBUFF_START_IDX(sample_buffer),
		   sizeof(sample_t) * SBUFF_LEN_UNTIL_WRAP(sample_buffer));
	memcpy(dst + SBUFF_LEN_UNTIL_WRAP(sample_buffer), sample_buffer->buff,
		   sizeof(sample_t) * (count - SBUFF_LEN_UNTIL_WRAP(sample_buffer)));
}

size_t sample_buffer_len(struct sample_buffer *sample_buffer) {
	return MIN(sample_buffer->buff_size, sample_buffer->total_sample_count);
}
