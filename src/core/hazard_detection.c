#include "hazard_detection.h"
#include "common/log.h"

static inline void flush_stage(hazard_flags_t* hazards, mips_core_stage_t stage) {
	switch (stage) {
		case MIPS_STAGE_IF:
		case MIPS_STAGE_ID:
		case MIPS_STAGE_EX:
			if (!hazards->stalls[stage]) { hazards->flushes[stage] = true; }
			break;
		default: log_assert_fail("Cannot flush stage %d\n", stage); break;
	}
}

static inline void stall_stage(hazard_flags_t* hazards, mips_core_stage_t stage) {
	flush_stage(hazards, stage);

	switch (stage) {
		case MIPS_STAGE_MEM: hazards->stalls[MIPS_STAGE_MEM] = true; ATTR_FALLTHROUGH;
		case MIPS_STAGE_EX: hazards->stalls[MIPS_STAGE_EX] = true; ATTR_FALLTHROUGH;
		case MIPS_STAGE_ID: hazards->stalls[MIPS_STAGE_ID] = true; ATTR_FALLTHROUGH;
		case MIPS_STAGE_IF: hazards->stalls[MIPS_STAGE_IF] = true; break;
		default: log_assert_fail("Cannot stall stage %d\n", stage);
	}
}

hazard_flags_t detect_hazards(
    const mips_pipeline_regs_t* regs, const mips_pipeline_regs_t* next_regs,
    const mips_config_t* config) {
	hazard_flags_t hazards = {
	    .flushes = {false},
	    .stalls  = {false},
	};

	// Load/Store data hazard
	const bool mem_load = regs->ex_mem.access_type == MEM_ACCESS_READ_SIGNED ||
	                      regs->ex_mem.access_type == MEM_ACCESS_READ_UNSIGNED;
	const mips_reg_idx_t mem_wb_reg = regs->ex_mem.mem_wb.reg;
	if (mem_load && mem_wb_reg != 0 &&
	    (regs->id_ex.reg_rs == mem_wb_reg ||
	     (regs->id_ex.reg_rt == mem_wb_reg && regs->id_ex.alu_b_src == ALU_SRC_DATA_B))) {
		stall_stage(&hazards, MIPS_STAGE_EX);
	}

	// Branch hazard - Branch operand is being calculated in EX so stall
	const mips_reg_idx_t ex_wb_reg = regs->id_ex.ex_mem.mem_wb.reg;
	const mips_reg_idx_t id_rs     = next_regs->id_ex.reg_rs;
	const mips_reg_idx_t id_rt     = next_regs->id_ex.reg_rt;
	if ((id_rs == ex_wb_reg || id_rt == ex_wb_reg) && ex_wb_reg != 0) {
		if (next_regs->id_ex.eval_branch) { stall_stage(&hazards, MIPS_STAGE_ID); }
	}

	// Branch hazard - Branch operand is being loaded from memory
	if ((id_rs == mem_wb_reg || id_rt == mem_wb_reg) && mem_wb_reg != 0) {
		if (next_regs->id_ex.eval_branch && mem_load) { stall_stage(&hazards, MIPS_STAGE_ID); }
	}

	// Jump/Branch control hazard - pc is updated in core.c
	if (regs->id_ex.branch) {
		if (!config->delay_slots) { flush_stage(&hazards, MIPS_STAGE_ID); }
	}

	// Flush on exceptions
	if (next_regs->id_ex.ex_mem.mem_wb.metadata.exception.raised) {
		stall_stage(&hazards, MIPS_STAGE_IF);
	}
	if (next_regs->ex_mem.mem_wb.metadata.exception.raised) {
		flush_stage(&hazards, MIPS_STAGE_ID);
		stall_stage(&hazards, MIPS_STAGE_IF);
	}
	if (next_regs->mem_wb.metadata.exception.raised) {
		flush_stage(&hazards, MIPS_STAGE_EX);
		stall_stage(&hazards, MIPS_STAGE_IF);
	}

	return hazards;
}
