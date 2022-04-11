#include "instruction.h"

const char* mips_instr_name(uint32_t instruction) {
	const mips_opcode_t opc   = EXTRACT_BITS(31, 26, instruction);
	const mips_funct_t  funct = EXTRACT_BITS(5, 0, instruction);

	switch (opc) {
		case MIPS_OPC_R_FMT:
			switch (funct) {
				case MIPS_FUNCT_NOP: return "nop";
				case MIPS_FUNCT_ADDU: return "addu";
				case MIPS_FUNCT_AND: return "and";
				case MIPS_FUNCT_OR: return "or";
				case MIPS_FUNCT_BREAK: return "break";
				default: return "unknown";
			}

		case MIPS_OPC_ADDIU: return "addiu";
		case MIPS_OPC_ANDI: return "andi";
		case MIPS_OPC_ORI: return "ori";

		case MIPS_OPC_BEQ: return "beq";
		case MIPS_OPC_BNE: return "bne";

		case MIPS_OPC_SH: return "sh";
		case MIPS_OPC_LHU: return "lhu";

		case MIPS_OPC_J: return "j";

		default: return "unknown";
	}
}
