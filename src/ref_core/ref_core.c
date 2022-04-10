#include "ref_core.h"
#include <inttypes.h>
#include "util/instruction.h"
#include "util/log.h"

void ref_core_init(mips_ref_core_t* core, mips_config_t config) {
	*core = (mips_ref_core_t){
	    .state =
	        {
	            .pc  = 0,
	            .gpr = {0},
	        },

	    .cycle = 0,

	    .config = config,

	    .branch_after = false,
	    .branch_dest  = 0x00000000,
	};
}

// mips_retire_metadata_t ref_core_run(mips_ref_core_t* core) {
//     while (!(trap = ref_core_cycle(core))) {}
//     return trap;
// }

mips_retire_metadata_t ref_core_run_one(mips_ref_core_t* core) {
	while (true) {
		mips_retire_metadata_t retire = ref_core_cycle(core);
		if (retire.active) { return retire; }
	}
}

mips_retire_metadata_t ref_core_cycle(mips_ref_core_t* core) {
	mips_retire_metadata_t metadata = {
	    .address            = core->state.pc,
	    .active             = true,
	    .instruction_number = core->cycle,
	    .cycle              = core->cycle,
	};
	core->cycle++;

	if (core->state.pc >= core->config.instr_mem.size) {
		return metadata;
		// return MIPS_TRAP_INSTR_PAGE_FAULT;
	}

	uint32_t instr = *span_e(core->config.instr_mem, core->state.pc);
	instr |= (uint32_t) *span_e(core->config.instr_mem, core->state.pc + 1) << 8;
	instr |= (uint32_t) *span_e(core->config.instr_mem, core->state.pc + 2) << 16;
	instr |= (uint32_t) *span_e(core->config.instr_mem, core->state.pc + 3) << 24;
	metadata.instruction = instr;
	uint32_t next_pc     = core->branch_after ? core->branch_dest : core->state.pc + 4;
	core->branch_after   = false;

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

	// J type
	const uint32_t jump_address = EXTRACT_BITS(25, 0, instr);

	switch (opc) {
		case MIPS_OPC_R_FMT: {
			switch (funct) {
				case MIPS_FUNCT_NOP: break;

				case MIPS_FUNCT_ADDU: *rd = *rs + *rt; break;
				case MIPS_FUNCT_AND: *rd = *rs & *rt; break;
				case MIPS_FUNCT_OR: *rd = *rs | *rt; break;

				default:
					// return MIPS_TRAP_UNKNOWN_INSTR;
					return metadata;
			}
		} break;
		case MIPS_OPC_ADDIU: *rt = *rs + s_imm; break;

		case MIPS_OPC_ANDI: *rt = *rs & z_imm; break;
		case MIPS_OPC_ORI: *rt = *rs | z_imm; break;

		case MIPS_OPC_LHU: {
			// TODO: Trap on invalid access (page fault or unaligned)
			const uint32_t load_address = *rs + s_imm;

			*rt = (uint32_t) INSERT_BITS(7, 0, *span_e(core->config.data_mem, load_address)) |
			      (uint32_t) INSERT_BITS(15, 8, *span_e(core->config.data_mem, load_address + 1));
		} break;

		case MIPS_OPC_BEQ:
		case MIPS_OPC_BNE: {
			const bool     branch = (opc == MIPS_OPC_BEQ) == (*rs == *rt);
			const uint32_t target = next_pc + (s_imm << 2);
			if (branch) {
				if (core->config.delay_slots) {
					core->branch_after = true;
					core->branch_dest  = target;
				} else {
					next_pc = target;
				}
			}
		} break;

		case MIPS_OPC_SH: {
			// TODO: Trap on invalid access (page fault or unaligned)
			const uint32_t store_address                      = *rs + s_imm;
			*span_e(core->config.data_mem, store_address)     = EXTRACT_BITS(7, 0, *rt);
			*span_e(core->config.data_mem, store_address + 1) = EXTRACT_BITS(15, 8, *rt);
		} break;

		case MIPS_OPC_J: {
			// Pseudodirect addressing
			const uint32_t target = ((core->state.pc + 4) & 0xF0000000) | jump_address << 2;
			if (core->config.delay_slots) {
				core->branch_after = true;
				core->branch_dest  = target;
			} else {
				next_pc = target;
			}
		} break;

		default:
			// return MIPS_TRAP_UNKNOWN_INSTR;
			return metadata;
	}

	core->state.gpr[0] = 0;

	core->state.pc = next_pc;
	return metadata;
}
