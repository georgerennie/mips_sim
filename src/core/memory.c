#include "memory.h"
#include "util/esbmc_util.h"
#include "util/log.h"

// Load a byte from a larger word
static inline uint32_t load_byte(span_t data_mem, uint32_t base_addr, uint8_t byte) {
	log_assert_lti(byte, 4);
	return (uint32_t) *span_e(data_mem, base_addr + byte) << (8 * byte);
}

mem_wb_reg_t memory(const ex_mem_reg_t* ex_mem, span_t data_mem) {
	mem_wb_reg_t   mem_wb = ex_mem->mem_wb;
	const uint32_t addr   = mem_wb.result;
	const uint8_t  bytes  = ex_mem->bytes;

	if (ex_mem->access_type != MEM_ACCESS_NONE) {
		log_assert_fmt(bytes == 1 || bytes == 2 || bytes == 4, "bytes: %d\n", bytes);

		// TODO: Replace this with a trap
		esbmc_assume((uint64_t) addr + bytes < data_mem.size);
	}

	switch (ex_mem->access_type) {
		case MEM_ACCESS_READ_SIGNED: {
			// Currently only LW is supported
			log_assert_eqi(bytes, 4);

			// Load 0-4 bytes from memory little endian
			uint32_t load_val = load_byte(data_mem, addr, 0);
			load_val |= load_byte(data_mem, addr, 1);
			load_val |= load_byte(data_mem, addr, 2);
			load_val |= load_byte(data_mem, addr, 3);

			// Sign extend
			switch (bytes) {
				case 1: load_val = (uint32_t) (int8_t) load_val; break;
				case 2: load_val = (uint32_t) (int16_t) load_val; break;
			}

			mem_wb.result = load_val;
		} break;

		default: log_assert_eqi(bytes, 0); break;
	}

	return mem_wb;
}

void ex_mem_reg_init(ex_mem_reg_t* ex_mem) {
	ex_mem->access_type = MEM_ACCESS_NONE;
	ex_mem->bytes       = 0;
	mem_wb_reg_init(&ex_mem->mem_wb);
}
