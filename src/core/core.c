#include "core.h"
#include "instruction_decoder.h"
#include "util/log.h"

void mips_core_init(mips_core_t* core, span_t instr_mem, span_t data_mem) {
	mips_state_init(&core->state);

	core->instr_mem = instr_mem;
	core->data_mem  = data_mem;

	core->stalls[0] = false;
	for (size_t i = 1; i < sizeof(core->stalls) / sizeof(*core->stalls); i++) {
		core->stalls[i] = true;
	}

	core->trap       = MIPS_TRAP_NONE;
	core->trap_stage = MIPS_STAGE_IF;

	core->decoded_instruction = 0;
	execute_bundle_init(&core->exec_bundle);
	memory_access_bundle_init(&core->mem_bundle);
	writeback_bundle_init(&core->wb_bundle);
}

mips_trap_t mips_core_run(mips_core_t* core) {
	mips_trap_t trap;
	while (!(trap = mips_core_cycle(core))) {}
	return trap;
}

static void propagate_stalls(mips_core_t* core) {
	log_assert(sizeof(core->stalls) / sizeof(*core->stalls) == 5);
	core->stalls[4] = core->stalls[3];
	core->stalls[3] = core->stalls[2];
	core->stalls[2] = core->stalls[1];
	core->stalls[1] = core->stalls[0];
	core->stalls[0] = MIPS_TRAP_NONE;
}

static inline bool is_trapped(const mips_core_t* core, mips_core_stage_t stage) {
	return core->trap_stage > stage;
}

mips_trap_t mips_core_cycle(mips_core_t* core) {
	// Register writeback - Do this before anything else as in real hardware
	// WB would occur in the first half of the cycle, and ID in the second half
	if (!core->stalls[4]) { writeback(&core->state, &core->wb_bundle); }

	// Start next state as clone of current state
	mips_core_t next_state = *core;
	propagate_stalls(&next_state);
	if (next_state.trap_stage > MIPS_STAGE_IF) { next_state.trap_stage += 1; }

	// Instruction Fetch
	log_assert(core->state.pc % 4 == 0); // Check that the pc is word aligned
	if (!(core->stalls[0] || is_trapped(core, MIPS_STAGE_IF))) {
		if (core->state.pc >= core->instr_mem.size) {
			next_state.trap |= MIPS_TRAP_INSTR_PAGE_FAULT;
			next_state.trap_stage = MIPS_STAGE_ID;
		} else {
			log_assert(core->state.pc < core->instr_mem.size); // Check read is valid
			// Read little endian word
			uint32_t instr = *span_e(core->instr_mem, core->state.pc);
			instr |= (uint32_t) *span_e(core->instr_mem, core->state.pc + 1) << 8;
			instr |= (uint32_t) *span_e(core->instr_mem, core->state.pc + 2) << 16;
			instr |= (uint32_t) *span_e(core->instr_mem, core->state.pc + 3) << 24;
			next_state.decoded_instruction = instr;
			next_state.state.pc            = core->state.pc + 4;
		}
	}

	// Decode / Register Fetch
	if (!(core->stalls[1] || is_trapped(core, MIPS_STAGE_ID))) {
		decode_result_t dec    = decode_instruction(core, core->decoded_instruction);
		next_state.exec_bundle = dec.exec;
		if (dec.trap) {
			next_state.trap |= dec.trap;
			next_state.trap_stage = MIPS_STAGE_EX;
			// TODO: undo increment in PC from following instruction being in IF
		}
	}

	// Execute
	if (!(core->stalls[2] || is_trapped(core, MIPS_STAGE_EX))) {
		next_state.mem_bundle = execute_instruction(
		    &core->exec_bundle, core->mem_bundle.wb.value, core->wb_bundle.value);
	}

	// Memory Access
	if (!(core->stalls[3] || is_trapped(core, MIPS_STAGE_MEM))) {
		next_state.wb_bundle = access_memory(&core->mem_bundle, core->data_mem);
	}

	mips_trap_t ret;
	if (core->trap_stage >= MIPS_STAGE_WB) {
		next_state.trap_stage = 0;
		next_state.trap       = MIPS_TRAP_NONE;
		ret                   = core->trap;
	} else {
		ret = MIPS_TRAP_NONE;
	}

	// Update state
	*core = next_state;

	return ret;
}
