#include "core.h"
#include "instruction_decoder.h"
#include "util/log.h"

void mips_core_init(mips_core_t* core, span_t instr_mem, span_t data_mem) {
	mips_state_init(&core->state);

	core->cycle = 0;

	core->instr_mem = instr_mem;
	core->data_mem  = data_mem;

	core->stalls[0] = false;
	for (size_t i = 1; i < NUM_ELEMS(core->stalls); i++) { core->stalls[i] = true; }

	for (size_t i = 0; i < NUM_ELEMS(core->instr_addrs); i++) { core->instr_addrs[i] = 0; }

	core->trap       = MIPS_TRAP_NONE;
	core->trap_stage = MIPS_STAGE_NONE;

	core->decoded_instruction = 0;
	execute_bundle_init(&core->exec_bundle);
	memory_access_bundle_init(&core->mem_bundle);
	writeback_bundle_init(&core->wb_bundle);
}

mips_result_t mips_core_run(mips_core_t* core) {
	for (;;) {
		mips_result_t result = mips_core_cycle(core);
		if (result.trap != MIPS_TRAP_NONE) { return result; }
	}
}

static inline bool stage_active(const mips_core_t* core, mips_core_stage_t stage) {
	return !(core->stalls[stage] || core->trap_stage >= stage);
}

// Propagate tag data that passes down the pipeline - stalls and instruction
// addresses
static void propagate_tags(mips_core_t* core) {
	// There should be one of each of these for each stage of the pipeline
	log_assert_eqi(NUM_ELEMS(core->stalls), NUM_ELEMS(core->instr_addrs));

	for (mips_core_stage_t i = MIPS_STAGE_WB; i > MIPS_STAGE_IF; i--) {
		core->stalls[i] = core->stalls[i - 1];
		if (stage_active(core, i)) { core->instr_addrs[i] = core->instr_addrs[i - 1]; }
	}
	core->stalls[0] = false;
}

mips_result_t mips_core_cycle(mips_core_t* core) {
	log_dbg("Starting cycle %d. Trap stage is %d\n", core->cycle, core->trap_stage);
	core->cycle++;

	// Register writeback - Do this before anything else as in real hardware
	// WB would occur in the first half of the cycle, and ID in the second half
	if (stage_active(core, MIPS_STAGE_WB)) { writeback(&core->state, &core->wb_bundle); }

	// Start next state as clone of current state
	mips_core_t next_state = *core;
	propagate_tags(&next_state);
	if (next_state.trap_stage > MIPS_STAGE_NONE && next_state.trap_stage <= MIPS_STAGE_WB) {
		next_state.trap_stage += 1;
	}

	// Instruction Fetch
	log_assert_eqi(core->state.pc % 4, 0); // Check that the pc is word aligned
	if (stage_active(core, MIPS_STAGE_IF)) {
		if (core->state.pc >= core->instr_mem.size) {
			log_dbgi("Instruction memory page fault detected\n");
			next_state.trap |= MIPS_TRAP_INSTR_PAGE_FAULT;
			next_state.trap_stage = MIPS_STAGE_ID;
		} else {
			log_assert_lti(core->state.pc, core->instr_mem.size); // Check read is valid
			next_state.instr_addrs[MIPS_STAGE_ID] = core->state.pc;
			// Read little endian word
			uint32_t instr = *span_e(core->instr_mem, core->state.pc);
			instr |= (uint32_t) *span_e(core->instr_mem, core->state.pc + 1) << 8;
			instr |= (uint32_t) *span_e(core->instr_mem, core->state.pc + 2) << 16;
			instr |= (uint32_t) *span_e(core->instr_mem, core->state.pc + 3) << 24;
			next_state.decoded_instruction = instr;
			next_state.state.pc            = core->state.pc + 4;
			log_dbgi("Fetched instruction\n");
		}
	}

	// Decode / Register Fetch
	if (stage_active(core, MIPS_STAGE_ID)) {
		decode_result_t dec    = decode_instruction(core, core->decoded_instruction);
		next_state.exec_bundle = dec.exec;
		if (dec.trap) {
			// TODO: What happens if the instruction traps at the same time as
			// an instruction traps in IF
			next_state.trap       = dec.trap;
			next_state.trap_stage = MIPS_STAGE_EX;
			// TODO: undo increment in PC from following instruction being in IF
		}
	}

	// Execute
	if (stage_active(core, MIPS_STAGE_EX)) {
		next_state.mem_bundle = execute_instruction(
		    &core->exec_bundle, core->mem_bundle.wb.value, core->wb_bundle.value);
	}

	// Memory Access
	if (stage_active(core, MIPS_STAGE_MEM)) {
		next_state.wb_bundle = access_memory(&core->mem_bundle, core->data_mem);
	}

	mips_result_t result = {
	    .instr_retired = stage_active(core, MIPS_STAGE_WB),
	    .instr_addr    = core->instr_addrs[MIPS_STAGE_WB],
	};

	if (core->trap_stage >= MIPS_STAGE_WB) {
		next_state.trap_stage = 0;
		next_state.trap       = MIPS_TRAP_NONE;
		result.trap           = core->trap;
	} else {
		result.trap = MIPS_TRAP_NONE;
	}

	if (result.instr_retired) {
		log_dbgi("Retired instruction at address 0x%08X\n", result.instr_addr);
	}

	// Update state
	*core = next_state;
	return result;
}
