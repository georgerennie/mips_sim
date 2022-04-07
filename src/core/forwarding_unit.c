#include "forwarding_unit.h"
#include <stdint.h>
#include "util/log.h"

static inline ex_fwd_src_t get_fwd_src(const mips_core_t* core, uint8_t reg) {
	if (reg == 0) { return EX_FWD_SRC_NONE; }
	if (reg == core->ex_mem.mem_wb.reg) { return EX_FWD_SRC_EXEC; }
	if (reg == core->mem_wb.reg) { return EX_FWD_SRC_MEM; }
	return EX_FWD_SRC_NONE;
}

// Setup forwarded values/control logic for EX
void setup_forwards(mips_core_t* core) {
	// Pass through forwarded values
	core->id_ex.ex_mem_result = core->ex_mem.mem_wb.result;
	core->id_ex.mem_wb_result = core->mem_wb.result;
	log_dbgi("%d %d\n", core->id_ex.ex_mem_result, core->id_ex.mem_wb_result);

	core->id_ex.fwd_a = get_fwd_src(core, core->id_ex.reg_rs);
	core->id_ex.fwd_b = get_fwd_src(core, core->id_ex.reg_rt);
}
