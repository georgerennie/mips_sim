#include "hazard_detection.h"
#include "util/log.h"

static inline void flush_stage(hazard_flags_t* hazards, mips_core_stage_t stage) {
	switch (stage) {
		case MIPS_STAGE_IF:
		case MIPS_STAGE_ID:
		case MIPS_STAGE_EX: hazards->flushes[stage] = true; break;
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

hazard_flags_t detect_hazards(mips_core_t* core) {
	hazard_flags_t hazards = {
	    .flushes = {false},
	    .stalls  = {false},
	};

	// Load/Store data hazard
	const bool mem_load = core->regs.ex_mem.access_type == MEM_ACCESS_READ_SIGNED ||
	                      core->regs.ex_mem.access_type == MEM_ACCESS_READ_UNSIGNED;
	const uint8_t wb_reg = core->regs.ex_mem.mem_wb.reg;
	if (core->regs.id_ex.reg_rs == wb_reg || core->regs.id_ex.reg_rt == wb_reg) {
		if (mem_load) { stall_stage(&hazards, MIPS_STAGE_EX); }
	}

	// Jump/Branch control hazard - pc is updated in core.c
	if (core->regs.id_ex.branch) {
		if (!core->delay_slots) { flush_stage(&hazards, MIPS_STAGE_ID); }
	}

	return hazards;
}
