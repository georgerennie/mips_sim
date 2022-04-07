#include "core.h"
#include "instruction_decode.h"
#include "instruction_fetch.h"
#include "util/log.h"

void mips_core_init(mips_core_t* core, span_t instr_mem, span_t data_mem) {
	mips_state_init(&core->state);

	core->cycle = 0;

	core->instr_mem = instr_mem;
	core->data_mem  = data_mem;

	core->if_id_instruction = 0;
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

	// Start next state as clone of current state
	mips_core_t next_state = *core;

	// Instruction Fetch
	next_state.if_id_instruction = instruction_fetch(&core->state, core->instr_mem);
	next_state.state.pc          = core->state.pc + 4;

	// Decode / Register Fetch
	next_state.id_ex = instruction_decode(core->if_id_instruction, &core->state);

	// Execute
	next_state.ex_mem = execute(&core->id_ex);

	// Memory Access
	next_state.mem_wb = memory(&core->ex_mem, core->data_mem);

	// Update state
	*core = next_state;

	mips_result_t result = {0};
	return result;
}
