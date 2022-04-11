#include "common/log.h"
#include "core.h"

inline void writeback(const mem_wb_reg_t* mem_wb, mips_state_t* arch_state) {
	log_assert_lti(mem_wb->reg, 32);
	if (mem_wb->reg > 0) { log_dbgi("Writing %d to reg %d\n", mem_wb->result, mem_wb->reg); }
	if (mem_wb->reg > 0) { arch_state->gpr[mem_wb->reg] = mem_wb->result; }
}
