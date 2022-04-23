#include "forwarding_unit.h"
#include <stdint.h>
#include "common/log.h"

uint32_t get_fwd_value(const mips_pipeline_regs_t* regs, mips_reg_idx_t reg, uint32_t fetched_val) {
	if (reg == (mips_reg_idx_t) 0) { return fetched_val; }
	if (reg == regs->ex_mem.mem_wb.reg && regs->ex_mem.access_type != MEM_ACCESS_READ_SIGNED &&
	    regs->ex_mem.access_type != MEM_ACCESS_READ_UNSIGNED) {
		return regs->ex_mem.mem_wb.result;
	}
	if (reg == regs->mem_wb.reg) { return regs->mem_wb.result; }
	return fetched_val;
}
