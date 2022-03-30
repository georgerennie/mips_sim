#include <assert.h>
#include "memory_access.h"
#include "esbmc_util.h"

// Load a byte from a larger word
static inline uint32_t load_byte(span_t data_mem, uint32_t base_addr, uint8_t byte) {
	assert(byte < 4);
	return (uint32_t) *span_e(data_mem, base_addr + byte) << (8 * byte);
}

writeback_bundle_t access_memory(const memory_access_bundle_t* bundle, span_t data_mem) {
	writeback_bundle_t wb    = bundle->wb;
	const uint32_t     addr  = bundle->wb.value;
	const uint8_t      bytes = bundle->bytes;

	if (bundle->access_type != MEM_ACCESS_NONE) {
		assert(bytes == 1 || bytes == 2 || bytes == 4);

		// TODO: Replace this with a trap
		__ESBMC_assume((uint64_t) addr + bytes < data_mem.size);
	}

	switch (bundle->access_type) {
		case MEM_ACCESS_READ_UNSIGNED:
		case MEM_ACCESS_READ_SIGNED: {
			// Load 0-4 bytes from memory
			// This is weird but ESBMC likes it. TODO: Can it be made nicer?
			uint32_t load_val = 0;
			if (bytes >= 1) { load_val |= load_byte(data_mem, addr, 0); }
			if (bytes >= 2) { load_val |= load_byte(data_mem, addr, 1); }
			if (bytes >= 4) {
				load_val |= load_byte(data_mem, addr, 2);
				load_val |= load_byte(data_mem, addr, 3);
			}

			// Sign extend if needed
			if (bundle->access_type == MEM_ACCESS_READ_SIGNED) {
				switch (bytes) {
					case 1: load_val = (uint32_t) (int8_t) load_val; break;
					case 2: load_val = (uint32_t) (int16_t) load_val; break;
				}
			}
			wb.value = load_val;
		} break;

		default: assert(bytes == 0); break;
	}

	return wb;
}

inline void memory_access_bundle_init(memory_access_bundle_t* bundle) {
	bundle->access_type = MEM_ACCESS_NONE;
	bundle->bytes = 0;
	writeback_bundle_init(&bundle->wb);
}
