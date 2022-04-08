#include "memory.h"
#include "util/esbmc_util.h"
#include "util/log.h"

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
		case MEM_ACCESS_READ_UNSIGNED: {
			// Currently only LHU is supported
			log_assert_eqi(bytes, 2);

			// Load 2 bytes from memory little endian
			uint32_t load_val = (uint32_t) INSERT_BITS(7, 0, *span_e(data_mem, addr)) |
			                    (uint32_t) INSERT_BITS(15, 8, *span_e(data_mem, addr + 1));
			mem_wb.result = load_val;
		} break;

		case MEM_ACCESS_WRITE: {
			// Currently only SH is supported
			log_msg("here %d\n", ex_mem->data_rt);
			log_assert_eqi(bytes, 2);

			*span_e(data_mem, addr)     = EXTRACT_BITS(7, 0, ex_mem->data_rt);
			*span_e(data_mem, addr + 1) = EXTRACT_BITS(15, 8, ex_mem->data_rt);
			mem_wb.reg                  = 0;
		} break;

		case MEM_ACCESS_READ_SIGNED: log_assert_fail("Signed read not supported"); break;
		case MEM_ACCESS_NONE: break;
	}

	return mem_wb;
}

void ex_mem_reg_init(ex_mem_reg_t* ex_mem) {
	ex_mem->access_type = MEM_ACCESS_NONE;
	ex_mem->bytes       = 0;
	ex_mem->data_rt     = 0;
	mem_wb_reg_init(&ex_mem->mem_wb);
}
