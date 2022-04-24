#include "common/log.h"
#include "core.h"

if_id_reg_t instruction_fetch(const mips_state_t* arch_state, span_t instr_mem) {
	if_id_reg_t if_id;
	if_id_reg_init(&if_id);
	if_id.metadata.address = arch_state->pc;

	const uint32_t offset_address = arch_state->pc - TEXT_BASE_ADDR;

	log_assert_eqi(offset_address % 4, 0); // Check that the pc is word aligned
	if (offset_address >= instr_mem.size) {
		if_id.metadata.exception.raised     = true;
		if_id.metadata.exception.cause      = MIPS_EXCP_ADDRL;
		if_id.metadata.exception.bad_v_addr = arch_state->pc;
		return if_id;
	}

	// Read big endian word
	uint32_t instr = *span_e(instr_mem, offset_address + 3);
	instr |= (uint32_t) *span_e(instr_mem, offset_address + 2) << 8;
	instr |= (uint32_t) *span_e(instr_mem, offset_address + 1) << 16;
	instr |= (uint32_t) *span_e(instr_mem, offset_address) << 24;
	if_id.metadata.instruction = instr;
	if_id.metadata.active      = true;
	return if_id;
}
