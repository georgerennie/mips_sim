#include "instruction_fetch.h"
#include "util/log.h"

uint32_t instruction_fetch(const mips_state_t* arch_state, span_t instr_mem) {
	log_assert_eqi(arch_state->pc % 4, 0); // Check that the pc is word aligned
	if (arch_state->pc >= instr_mem.size) {
		log_dbgi("Instruction memory page fault detected\n");
		return 0;
	}

	log_assert_lti(arch_state->pc, instr_mem.size); // Check read is valid
	// Read little endian word
	uint32_t instr = *span_e(instr_mem, arch_state->pc);
	instr |= (uint32_t) *span_e(instr_mem, arch_state->pc + 1) << 8;
	instr |= (uint32_t) *span_e(instr_mem, arch_state->pc + 2) << 16;
	instr |= (uint32_t) *span_e(instr_mem, arch_state->pc + 3) << 24;

	log_dbgi("Fetched instruction\n");
	return instr;
}
