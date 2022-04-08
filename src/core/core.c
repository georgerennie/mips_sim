#include "core.h"
#include "forwarding_unit.h"
#include "hazard_detection.h"
#include "instruction_fetch.h"
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

	// Register writeback - Do this before anything else as in real hardware
	// WB would occur in the first half of the cycle, and ID in the second half
	writeback(&core->regs.mem_wb, &core->state);

	// Propagate forwarded values
	setup_forwards(&core->regs);

	// Values to writeback
	mips_pipeline_regs_t next_regs;

	// Instruction Fetch
	next_regs.if_id = instruction_fetch(&core->state, core->instr_mem);

	// Decode / Register Fetch
	next_regs.id_ex = instruction_decode(&core->regs.if_id, &core->state);

	// Execute
	next_regs.ex_mem = execute(&core->regs.id_ex);

	// Memory Access
	next_regs.mem_wb = memory(&core->regs.ex_mem, core->data_mem);

	// Detect and deal with hazards/branches
	handle_hazards(core, &next_regs);

	// Update registers dependent on stalls
	if (!next_regs.stalls[MIPS_STAGE_IF]) {
		core->state.pc   = (!next_regs.stalls[MIPS_STAGE_ID] && next_regs.id_ex.branch)
		                       ? next_regs.id_ex.branch_address
		                       : core->state.pc + 4;
		core->regs.if_id = next_regs.if_id;
	}
	if (!next_regs.stalls[MIPS_STAGE_ID]) { core->regs.id_ex = next_regs.id_ex; }
	if (!next_regs.stalls[MIPS_STAGE_EX]) { core->regs.ex_mem = next_regs.ex_mem; }
	if (!next_regs.stalls[MIPS_STAGE_MEM]) { core->regs.mem_wb = next_regs.mem_wb; }

	mips_result_t result = {0};
	return result;
}
