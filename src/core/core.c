#include "core.h"
#include <assert.h>

void mips_core_init(
    mips_core_t* core, uint32_t* instr_mem, size_t instr_mem_size, span_t data_mem) {
	mips_state_init(&core->state);

	core->instr_mem = instr_mem;
	core->instr_mem_size = instr_mem_size;
	core->data_mem       = data_mem;

	core->decoded_instruction = 0;
	execute_bundle_init(&core->exec_bundle);
	memory_access_bundle_init(&core->mem_bundle);
	writeback_bundle_init(&core->wb_bundle);
}

void mips_core_cycle(mips_core_t* core) {
	// Start next state as clone of current state
	mips_core_t next_state = *core;

	// Instruction Fetch
	// TODO: Exit gracefully, flushing the pipeline
	if (core->state.pc / 4 >= core->instr_mem_size) { return; }
	assert(core->state.pc % 4 == 0);                   // Check that the pc is word aligned
	assert(core->state.pc / 4 < core->instr_mem_size); // Check read is valid
	next_state.decoded_instruction = core->instr_mem[core->state.pc / 4];
	next_state.state.pc = core->state.pc + 4;

	// Decode / Register Fetch
	next_state.exec_bundle = decode_instruction(&core->state, core->decoded_instruction);

	// Execute
	next_state.mem_bundle = execute_instruction(&core->exec_bundle);

	// Memory Access
	next_state.wb_bundle = access_memory(&core->mem_bundle, core->data_mem);

	// Register writeback
	writeback(&next_state.state, &core->wb_bundle);

	// Update state
	*core = next_state;
}
