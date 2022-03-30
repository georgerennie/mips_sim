#include "core.h"
#include <assert.h>
#include <stdio.h>

void mips_core_init(
    mips_core_t* core, uint32_t* instr_mem, size_t instr_mem_size, span_t data_mem) {
	*core = (mips_core_t){
	    .instr_mem      = instr_mem,
	    .instr_mem_size = instr_mem_size,
	    .data_mem       = data_mem,

	    .decode_out.writeback_reg  = 0,
	    .execute_out.writeback_reg = 0,
	    .memory_out.writeback_reg  = 0,
	};
	mips_state_init(&core->state);
}

void mips_core_cycle(mips_core_t* core) {
	// Start next state as clone of current state
	mips_core_t next_state = *core;

	// Instruction Fetch
	// TODO: Exit gracefully, flushing the pipeline
	if (core->state.pc / 4 >= core->instr_mem_size) { return; }
	assert(core->state.pc % 4 == 0);                   // Check that the pc is word aligned
	assert(core->state.pc / 4 < core->instr_mem_size); // Check read is valid
	next_state.state.ir = core->instr_mem[core->state.pc / 4];
	next_state.state.pc = core->state.pc + 4;

	// Decode / Register Fetch
	next_state.decode_out = decode_instruction(&core->state);

	// Execute
	next_state.execute_out = execute_instruction(&core->decode_out);

	// Memory Access
	next_state.memory_out = access_memory(&core->execute_out, core->data_mem);

	// Register writeback
	gpr_write(&next_state.state, core->memory_out.writeback_reg, core->memory_out.writeback_value);

	// Update state
	*core = next_state;
}
