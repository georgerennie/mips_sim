#include "ref_core.h"
#include <inttypes.h>
#include "core/instruction.h"
#include "util/log.h"

void ref_core_init(mips_ref_core_t* core, span_t instr_mem, span_t data_mem) {
	mips_state_init(&core->state);

	core->instr_mem = instr_mem;
	core->data_mem  = data_mem;
}

mips_trap_t ref_core_run(mips_ref_core_t* core) {
	mips_trap_t trap;
	while (!(trap = ref_core_cycle(core))) {}
	return trap;
}

mips_trap_t ref_core_cycle(mips_ref_core_t* core) {
	if (core->state.pc >= core->instr_mem.size) { return MIPS_TRAP_INSTR_PAGE_FAULT; }

	uint32_t instr = *span_e(core->instr_mem, core->state.pc);
	instr |= (uint32_t) *span_e(core->instr_mem, core->state.pc + 1) << 8;
	instr |= (uint32_t) *span_e(core->instr_mem, core->state.pc + 2) << 16;
	instr |= (uint32_t) *span_e(core->instr_mem, core->state.pc + 3) << 24;
	uint32_t next_pc = core->state.pc + 4;

	const mips_opcode_t opc = EXTRACT_BITS(31, 26, instr);

	// R/I type
	uint32_t* rs = &core->state.gpr[EXTRACT_BITS(25, 21, instr)];
	uint32_t* rt = &core->state.gpr[EXTRACT_BITS(20, 16, instr)];

	// R type
	uint32_t* rd = &core->state.gpr[EXTRACT_BITS(15, 11, instr)];
	// const uint32_t shamt = &core->state.gpr[EXTRACT_BITS(10, 6, instr)];
	const mips_funct_t funct = EXTRACT_BITS(5, 0, instr);

	// I type
	const uint16_t imm   = EXTRACT_BITS(15, 0, instr);
	const uint32_t s_imm = (uint32_t) (int16_t) imm;  // Sign-extend
	const uint32_t z_imm = (uint32_t) (uint16_t) imm; // Zero-extend

	switch (opc) {
		case MIPS_OPC_R_FMT: {
			switch (funct) {
				case MIPS_FUNCT_ADDU: *rd = *rs + *rt; break;
				case MIPS_FUNCT_AND: *rd = *rs & *rt; break;
				case MIPS_FUNCT_OR: *rd = *rs | *rt; break;

				default: return MIPS_TRAP_UNKNOWN_INSTR;
			}
		} break;
		case MIPS_OPC_ADDIU: *rt = *rs + s_imm; break;

		case MIPS_OPC_ANDI: *rt = *rs & z_imm; break;
		case MIPS_OPC_ORI: *rt = *rs | z_imm; break;

		// TODO: implement load word and jump
		case MIPS_OPC_LW: break;
		case MIPS_OPC_J: break;

		default: return MIPS_TRAP_UNKNOWN_INSTR;
	}

	core->state.gpr[0] = 0;

	core->state.pc = next_pc;
	return MIPS_TRAP_NONE;
}