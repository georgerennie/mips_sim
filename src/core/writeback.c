#include "core.h"

inline void writeback(const mem_wb_reg_t* mem_wb, mips_state_t* arch_state) {
	gpr_write(arch_state, mem_wb->reg, mem_wb->result);
}

void mem_wb_reg_init(mem_wb_reg_t* mem_wb) {
	mem_wb->reg    = 0;
	mem_wb->result = 0;

	mem_wb->metadata.address     = 0x00000000;
	mem_wb->metadata.instruction = 0x00000000;
}
