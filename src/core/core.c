#include "core.h"
#include "hazard_detection.h"
#include "util/log.h"

void mips_core_init(mips_core_t* core, span_t instr_mem, span_t data_mem, bool delay_slots) {
	mips_state_init(&core->state);

	core->delay_slots = delay_slots;
	core->cycle       = 0;

	core->instr_mem = instr_mem;
	core->data_mem  = data_mem;

	if_id_reg_init(&core->regs.if_id);
	id_ex_reg_init(&core->regs.id_ex);
	ex_mem_reg_init(&core->regs.ex_mem);
	mem_wb_reg_init(&core->regs.mem_wb);
}

mips_result_t mips_core_run(mips_core_t* core) {
	for (;;) {
		// TODO: Catch traps here
		mips_core_cycle(core);
	}
}

mips_result_t mips_core_cycle(mips_core_t* core) {
	log_dbg("Starting cycle %d\n", core->cycle);
	core->cycle++;

	// ---------------- Pipeline stages ----------------------------------------

	// Register writeback - Do this before anything else as in real hardware
	// WB would occur in the first half of the cycle, and ID in the second half
	writeback(&core->regs.mem_wb, &core->state);

	// Instruction Fetch
	if_id_reg_t if_id = instruction_fetch(&core->state, core->instr_mem);

	// Decode / Register Fetch
	id_ex_reg_t id_ex = instruction_decode(&core->regs, &core->state);

	// Execute
	ex_mem_reg_t ex_mem = execute(&core->regs);

	// Memory Access
	mem_wb_reg_t mem_wb = memory(&core->regs.ex_mem, core->data_mem);

	// ---------------- Pipeline register writeback ----------------------------
	//
	// Detect hazards/branches
	hazard_flags_t hazards = detect_hazards(core, &id_ex);

	if (hazards.flushes[MIPS_STAGE_IF]) {
		if_id_reg_init(&core->regs.if_id);
	} else if (!hazards.stalls[MIPS_STAGE_IF]) {
		core->regs.if_id = if_id;
	}

	if (hazards.flushes[MIPS_STAGE_ID]) {
		id_ex_reg_init(&core->regs.id_ex);
	} else if (!hazards.stalls[MIPS_STAGE_ID]) {
		core->regs.id_ex = id_ex;
	}

	if (hazards.flushes[MIPS_STAGE_EX]) {
		ex_mem_reg_init(&core->regs.ex_mem);
	} else if (!hazards.stalls[MIPS_STAGE_EX]) {
		core->regs.ex_mem = ex_mem;
	}

	if (!hazards.stalls[MIPS_STAGE_MEM]) { core->regs.mem_wb = mem_wb; }

	// Update PC
	if (!hazards.flushes[MIPS_STAGE_ID] && core->regs.id_ex.branch) {
		core->state.pc = core->regs.id_ex.branch_address;
	} else if (!hazards.stalls[MIPS_STAGE_IF]) {
		core->state.pc += 4;
	}

	mips_result_t result = {0};
	return result;
}
