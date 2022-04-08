#include "forwarding_unit.h"
#include <stdint.h>
#include "util/log.h"

static inline ex_fwd_src_t get_fwd_src(const mips_pipeline_regs_t* regs, uint8_t reg) {
	if (reg == 0) { return EX_FWD_SRC_NONE; }
	if (reg == regs->ex_mem.mem_wb.reg) { return EX_FWD_SRC_EXEC; }
	if (reg == regs->mem_wb.reg) { return EX_FWD_SRC_MEM; }
	return EX_FWD_SRC_NONE;
}

// Setup forwarded values/control logic for EX
void setup_forwards(mips_pipeline_regs_t* regs) {
	// Pass through forwarded values
	regs->id_ex.ex_mem_result = regs->ex_mem.mem_wb.result;
	regs->id_ex.mem_wb_result = regs->mem_wb.result;

	regs->id_ex.fwd_a = get_fwd_src(regs, regs->id_ex.reg_rs);
	regs->id_ex.fwd_b = get_fwd_src(regs, regs->id_ex.reg_rt);
}
