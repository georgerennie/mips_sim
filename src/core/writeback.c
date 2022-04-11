#include "common/log.h"
#include "core.h"

inline void writeback(const mem_wb_reg_t* mem_wb, mips_state_t* arch_state) {
	if (mem_wb->reg > (mips_reg_idx_t) 0) {
		log_dbgi("Writing %d to reg %d\n", mem_wb->result, mem_wb->reg);
		arch_state->gpr[mem_wb->reg] = mem_wb->result;
	}
}
