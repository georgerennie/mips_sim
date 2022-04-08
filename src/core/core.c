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

	if_id_reg_init(&core->if_id);
	id_ex_reg_init(&core->id_ex);
	ex_mem_reg_init(&core->ex_mem);
	mem_wb_reg_init(&core->mem_wb);
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
	writeback(&core->mem_wb, &core->state);

	// Instruction Fetch
	if_id_reg_t if_id = instruction_fetch(&core->state, core->instr_mem);

	// Decode / Register Fetch
	id_ex_reg_t id_ex = instruction_decode(&core->if_id, &core->state);

	// Execute
	setup_forwards(core);
	ex_mem_reg_t ex_mem = execute(&core->id_ex);

	// Memory Access
	mem_wb_reg_t mem_wb = memory(&core->ex_mem, core->data_mem);

	// Detect and deal with hazards/branches
	handle_hazards(core);

	// Update registers dependent on stalls
	if (!core->stalls[MIPS_STAGE_IF]) {
		core->state.pc = core->id_ex.branch ? core->id_ex.branch_address : core->state.pc + 4;
		core->if_id    = if_id;
	}
	if (!core->stalls[MIPS_STAGE_ID]) { core->id_ex = id_ex; }
	if (!core->stalls[MIPS_STAGE_EX]) { core->ex_mem = ex_mem; }
	if (!core->stalls[MIPS_STAGE_MEM]) { core->mem_wb = mem_wb; }

	mips_result_t result = {0};
	return result;
}
