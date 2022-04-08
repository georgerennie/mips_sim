#include "hazard_detection.h"
#include "util/log.h"

static inline void flush_stage(mips_pipeline_regs_t* regs, mips_core_stage_t stage) {
	// To turn an instruction into a nop, if it hasnt yet been decoded, we just
	// replace it with a nop (0x00000000) and if it has, we just need to set
	// its writeback reg to 0 and its memory write to none
	ex_mem_reg_t* ex_mem = &regs->ex_mem;

	switch (stage) {
		case MIPS_STAGE_IF: if_id_reg_init(&regs->if_id); break;

		case MIPS_STAGE_ID: ex_mem = &regs->id_ex.ex_mem; FALLTHROUGH;

		case MIPS_STAGE_EX: {
			ex_mem->access_type = MEM_ACCESS_NONE;
			ex_mem->mem_wb.reg  = 0;
		} break;

		default: log_assert_fail("Cannot flush stage %d\n", stage);
	}
}

static inline void stall_stage(mips_pipeline_regs_t* regs, mips_core_stage_t stage) {
	flush_stage(regs, stage);

	switch (stage) {
		case MIPS_STAGE_MEM: regs->stalls[MIPS_STAGE_MEM] = true; FALLTHROUGH;
		case MIPS_STAGE_EX: regs->stalls[MIPS_STAGE_EX] = true; FALLTHROUGH;
		case MIPS_STAGE_ID: regs->stalls[MIPS_STAGE_ID] = true; FALLTHROUGH;
		case MIPS_STAGE_IF: regs->stalls[MIPS_STAGE_IF] = true; break;
		default: log_assert_fail("Cannot stall stage %d\n", stage);
	}
}

void handle_hazards(mips_core_t* core, mips_pipeline_regs_t* next_regs) {
	// Assume no stalls to start with. Unwound for ESBMC
	next_regs->stalls[MIPS_STAGE_IF]  = 0;
	next_regs->stalls[MIPS_STAGE_ID]  = 0;
	next_regs->stalls[MIPS_STAGE_EX]  = 0;
	next_regs->stalls[MIPS_STAGE_MEM] = 0;

	// Data hazard
	const bool mem_read = core->regs.ex_mem.access_type == MEM_ACCESS_READ_SIGNED ||
	                      core->regs.ex_mem.access_type == MEM_ACCESS_READ_UNSIGNED;
	const uint8_t wb_reg = core->regs.ex_mem.mem_wb.reg;
	if (mem_read && (next_regs->id_ex.reg_rs == wb_reg || next_regs->id_ex.reg_rt == wb_reg)) {
		stall_stage(next_regs, MIPS_STAGE_EX);
	}

	// Control hazard - pc is updated in core.c
	if (next_regs->id_ex.branch) {
		if (!core->delay_slots) { flush_stage(next_regs, MIPS_STAGE_IF); }
	}
}
