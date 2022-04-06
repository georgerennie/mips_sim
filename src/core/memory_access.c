#include "memory_access.h"
#include "util/esbmc_util.h"
#include "util/log.h"

// Load a byte from a larger word
static inline uint32_t load_byte(span_t data_mem, uint32_t base_addr, uint8_t byte) {
	log_assert_lti(byte, 4);
	return (uint32_t) *span_e(data_mem, base_addr + byte) << (8 * byte);
}

writeback_bundle_t access_memory(const memory_access_bundle_t* bundle, span_t data_mem) {
	writeback_bundle_t wb    = bundle->wb;
	const uint32_t     addr  = bundle->wb.value;
	const uint8_t      bytes = bundle->bytes;

	if (bundle->access_type != MEM_ACCESS_NONE) {
		log_assert_fmt(bytes == 1 || bytes == 2 || bytes == 4, "bytes: %d\n", bytes);

		// TODO: Replace this with a trap
		esbmc_assume((uint64_t) addr + bytes < data_mem.size);
	}

	switch (bundle->access_type) {
		case MEM_ACCESS_READ_SIGNED: {
			// Currently only SW is supported
			log_assert_eqi(bytes, 4);

			// Load 0-4 bytes from memory little endian
			uint32_t load_val = load_byte(data_mem, addr, 0);
			load_val |= load_byte(data_mem, addr, 1);
			load_val |= load_byte(data_mem, addr, 2);
			load_val |= load_byte(data_mem, addr, 3);

			switch (bytes) {
				case 1: load_val = (uint32_t) (int8_t) load_val; break;
				case 2: load_val = (uint32_t) (int16_t) load_val; break;
			}

			wb.value = load_val;
		} break;

		default: log_assert_eqi(bytes, 0); break;
	}

	return wb;
}

inline void memory_access_bundle_init(memory_access_bundle_t* bundle) {
	bundle->access_type = MEM_ACCESS_NONE;
	bundle->bytes       = 0;
	writeback_bundle_init(&bundle->wb);
}
