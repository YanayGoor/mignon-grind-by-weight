#include "config.h"

#include <hardware/flash.h>
#include <hardware/sync.h>

#include <mignon-grind-by-weight/defs.h>

#include <pico/printf.h>
#include <memory.h>

#define CONFIG_MAGIC_LEN (21)
#define MAGIC			 ("mignongrindbyweight2")

#define SLOT_SIZE ((sizeof(struct slot) / FLASH_SECTOR_SIZE + 1) * FLASH_SECTOR_SIZE)

#define NUM_BLOCKS (10) // arbitrary number
#define NUM_SLOTS  (FLASH_BLOCK_SIZE * NUM_BLOCKS / SLOT_SIZE)

#define CONFIG_AREA_START_OFFSET (PICO_FLASH_SIZE_BYTES - NUM_BLOCKS * FLASH_BLOCK_SIZE)
#define CONFIG_AREA_SIZE		 (NUM_BLOCKS * FLASH_BLOCK_SIZE)

struct slot {
	uint8_t magic[CONFIG_MAGIC_LEN]; /* to make sure the flash is really initialized and not just garbage */
	struct config config;
};

static_assert(sizeof(MAGIC) == CONFIG_MAGIC_LEN, "incorrect magic size");
static_assert(NUM_SLOTS > 0, "not enough space for slots");

const struct config default_config = {
	.handle_detection_sample_count = DEFAULT_HANDLE_DETECTION_SAMPLE_COUNT,
	.handle_weight = DEFAULT_HANDLE_WEIGHT,
	.handle_weight_range = DEFAULT_HANDLE_WEIGHT_RANGE,
	.target_coffee_weight = DEFAULT_TARGET_COFFEE_WEIGHT,
	.linear_estimator_sample_count = DEFAULT_LINEAR_ESTIMATOR_SAMPLE_COUNT,
	.scale_object_lifted_off_threshold = DEFAULT_SCALE_OBJECT_LIFTED_OFF_THRESHOLD,
	.scale_zero_sample_count = DEFAULT_SCALE_ZERO_SAMPLE_COUNT,
	.scale_to_grams_multiplier = DEFAULT_SCALE_TO_GRAMS_MULTIPLIER,
};

const struct config *cached_config = NULL;

static const struct slot *read_slot_by_idx(uint idx) {
	assert(idx < NUM_SLOTS);

	return (const struct slot *)(XIP_BASE + CONFIG_AREA_START_OFFSET + SLOT_SIZE * idx);
}

static void program_slot_by_idx(const struct slot *slot, uint idx) {
	assert(idx < NUM_SLOTS);

	static uint8_t buff[SLOT_SIZE] = {0};
	memcpy(buff, slot, sizeof(*slot));

	flash_range_program(CONFIG_AREA_START_OFFSET + SLOT_SIZE * idx, buff, sizeof(buff));
}

static void mark_slot_as_outdated(uint idx) {
	assert(idx < NUM_SLOTS);

	// set magic to zeros, this does not require erasure (which sets bits to 1)
	struct slot slot = {0};
	program_slot_by_idx(&slot, idx);
}

static void program_slot(struct config *config, uint idx) {
	assert(idx < NUM_SLOTS);

	// this should only be called on empty slots, so no need to erase
	struct slot slot = {.magic = MAGIC, .config = *config};
	program_slot_by_idx(&slot, idx);
}

static bool is_slot_free(const struct slot *slot) {
	for (size_t i = 0; i < sizeof(slot->magic); i++) {
		if (slot->magic[i] != 0xff) {
			return false;
		}
	}
	return true;
}

static bool is_slot_used(const struct slot *slot) {
	return !memcmp(&slot->magic, MAGIC, sizeof(slot->magic));
}

const struct config *read_config() {
	if (cached_config != NULL) {
		return cached_config;
	}

	for (uint i = 0; i < NUM_SLOTS; i++) {
		const struct slot *slot = read_slot_by_idx(i);
		if (is_slot_used(slot)) {
			cached_config = &slot->config;
			return &slot->config;
		}
	}

	cached_config = &default_config;
	return &default_config;
}

void save_config(struct config *config) {
	cached_config = NULL;

	uint32_t ints = save_and_disable_interrupts();

	for (uint i = 0; i < NUM_SLOTS; i++) {
		const struct slot *slot = read_slot_by_idx(i);
		if (is_slot_used(slot)) {
			mark_slot_as_outdated(i);
		}
		if (is_slot_free(slot)) {
			program_slot(config, i);
			goto cleanup;
		}
	}

	// no free slots found, erase entire block and start again.
	flash_range_erase(CONFIG_AREA_START_OFFSET, CONFIG_AREA_SIZE);
	program_slot(config, 0);

cleanup:
	restore_interrupts(ints);
}