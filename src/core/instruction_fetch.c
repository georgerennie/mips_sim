#include "core.h"
#include "util/log.h"

if_id_reg_t instruction_fetch(const mips_state_t* arch_state, span_t instr_mem) {
	if_id_reg_t if_id = {
	    .address     = arch_state->pc,
	    .instruction = 0x00000000,

	    .metadata = {.instruction = 0x00000000, .address = arch_state->pc, .active = false}};

	log_assert_eqi(arch_state->pc % 4, 0); // Check that the pc is word aligned
	if (arch_state->pc >= instr_mem.size) {
		log_dbgi("Instruction memory page fault detected\n");
		return if_id;
	}

	// Read little endian word
	uint32_t instr = *span_e(instr_mem, arch_state->pc);
	instr |= (uint32_t) *span_e(instr_mem, arch_state->pc + 1) << 8;
	instr |= (uint32_t) *span_e(instr_mem, arch_state->pc + 2) << 16;
	instr |= (uint32_t) *span_e(instr_mem, arch_state->pc + 3) << 24;
	if_id.instruction = instr;

	if_id.metadata.instruction = instr;
	if_id.metadata.active      = true;

	log_dbgi("Fetched instruction\n");
	return if_id;
}
