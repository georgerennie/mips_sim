#include "memory_access.h"
#include "assert.h"
#include "esbmc_util.h"

// Load a byte from a larger word
static inline uint32_t load_byte(span_t data_mem, uint32_t base_addr, uint8_t byte) {
	assert(byte < 4);
	return (uint32_t) *span_e(data_mem, base_addr + byte) << (8 * byte);
}

memory_access_bundle_t access_memory(const execution_bundle_t* exec, span_t data_mem) {
	const uint32_t addr  = exec->result;
	const uint8_t  bytes = exec->mem_bytes;

	memory_access_bundle_t bundle = {
	    .writeback_reg   = exec->writeback_reg,
	    .writeback_value = exec->result,
	};

	if (exec->mem_access_type != MEM_ACCESS_NONE) {
		assert(bytes == 1 || bytes == 2 || bytes == 4);
	}

	// TODO: Replace this with a trap
	__ESBMC_assume((uint64_t) addr + bytes < data_mem.size);

	switch (exec->mem_access_type) {
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
			if (exec->mem_access_type == MEM_ACCESS_READ_SIGNED) {
				switch (bytes) {
					case 1: load_val = (uint32_t) (int8_t) load_val; break;
					case 2: load_val = (uint32_t) (int16_t) load_val; break;
				}
			}
			bundle.writeback_value = load_val;
		} break;

		default: assert(exec->mem_bytes == 0); break;
	}
	return bundle;
}
