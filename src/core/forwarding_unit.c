#include "forwarding_unit.h"
#include <stdint.h>
#include "common/log.h"

uint32_t get_fwd_value(const mips_pipeline_regs_t* regs, uint8_t reg, uint32_t fetched_val) {
	if (reg == 0) { return fetched_val; }
	if (reg == regs->ex_mem.mem_wb.reg) { return regs->ex_mem.mem_wb.result; }
	if (reg == regs->mem_wb.reg) { return regs->mem_wb.result; }
	return fetched_val;
}
