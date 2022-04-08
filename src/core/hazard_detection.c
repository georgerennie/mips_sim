#include "hazard_detection.h"
#include <string.h>
#include "util/log.h"

static inline void flush_stage(mips_core_t* core, mips_core_stage_t stage) {
	// To turn an instruction into a nop, if it hasnt yet been decoded, we just
	// replace it with a nop (0x00000000) and if it has, we just need to set
	// its writeback reg to 0 and its memory write to none
	ex_mem_reg_t* ex_mem = &core->ex_mem;

	switch (stage) {
		case MIPS_STAGE_IF: if_id_reg_init(&core->if_id); break;

		case MIPS_STAGE_ID: ex_mem = &core->id_ex.ex_mem; __attribute__((fallthrough));

		case MIPS_STAGE_EX: {
			ex_mem->access_type = MEM_ACCESS_NONE;
			ex_mem->mem_wb.reg  = 0;
		} break;

		default: log_assert_fail("Cannot flush stage %d\n", stage);
	}
}

static inline void stall_stage(mips_core_t* core, mips_core_stage_t stage) {
	flush_stage(core, stage);

	switch (stage) {
		case MIPS_STAGE_MEM: core->stalls[MIPS_STAGE_MEM] = true; __attribute__((fallthrough));
		case MIPS_STAGE_EX: core->stalls[MIPS_STAGE_EX] = true; __attribute__((fallthrough));
		case MIPS_STAGE_ID: core->stalls[MIPS_STAGE_ID] = true; __attribute__((fallthrough));
		case MIPS_STAGE_IF: core->stalls[MIPS_STAGE_IF] = true; break;
		default: log_assert_fail("Cannot stall stage %d\n", stage);
	}
}

void handle_hazards(mips_core_t* core) {
	// Assume no stalls to start with
	memset(&core->stalls, 0, sizeof(core->stalls));

	// Data hazard
	const bool mem_read = core->ex_mem.access_type == MEM_ACCESS_READ_SIGNED ||
	                      core->ex_mem.access_type == MEM_ACCESS_READ_UNSIGNED;
	const uint8_t wb_reg = core->ex_mem.mem_wb.reg;
	if (mem_read && (core->id_ex.reg_rs == wb_reg || core->id_ex.reg_rt == wb_reg)) {
		stall_stage(core, MIPS_STAGE_EX);
	}

	// Control hazard
	if (core->id_ex.branch) {
		core->state.pc = core->id_ex.branch_address;
		if (!core->delay_slots) { flush_stage(core, MIPS_STAGE_IF); }
	} else {
		core->state.pc += 4;
	}
}
