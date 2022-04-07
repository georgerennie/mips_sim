#include "instruction_decode.h"
#include "instruction.h"
#include "util/log.h"
#include "util/util.h"

id_ex_reg_t instruction_decode(uint32_t instr, const mips_state_t* arch_state) {
	const mips_opcode_t opc = EXTRACT_BITS(31, 26, instr);

	// R/I type
	const uint8_t rs = EXTRACT_BITS(25, 21, instr);
	const uint8_t rt = EXTRACT_BITS(20, 16, instr);

	// R type
	const uint8_t rd = EXTRACT_BITS(15, 11, instr);
	// const uint8_t shamt = EXTRACT_BITS(10, 6, instr);
	const mips_funct_t funct = EXTRACT_BITS(5, 0, instr);

	// I type
	const uint16_t imm   = EXTRACT_BITS(15, 0, instr);
	const uint32_t s_imm = (uint32_t) (int16_t) imm;  // Sign-extend
	const uint32_t z_imm = (uint32_t) (uint16_t) imm; // Zero-extend

	// Initialise values to pass down the pipeline
	id_ex_reg_t id_ex;
	id_ex_reg_init(&id_ex);

	// Register read
	id_ex.data_rs = gpr_read(arch_state, rs);
	id_ex.data_rt = gpr_read(arch_state, rt);
	id_ex.reg_rs  = rs;
	id_ex.reg_rt  = rt;

	if (opc == MIPS_OPC_R_FMT) {
		// Use rt as data b source
		id_ex.alu_b_src = ALU_SRC_DATA_B;
		// Write back to rd
		id_ex.ex_mem.mem_wb.reg = rd;

		switch (funct) {
			case MIPS_FUNCT_ADDU: id_ex.alu_op = ALU_OP_ADD; break;

			case MIPS_FUNCT_AND: id_ex.alu_op = ALU_OP_AND; break;
			case MIPS_FUNCT_OR: id_ex.alu_op = ALU_OP_OR; break;

			default: log_dbgi("Unrecognised funct 0x%02X for r type instruction\n", funct); break;
		}
	}

	else if (opc == MIPS_OPC_J) {
		log_dbgi("J format instruction. Ignored\n");
	}

	else {
		// These should all be I format instructions

		// Use immediate as data b source
		id_ex.alu_b_src = ALU_SRC_IMM;
		// Sign extend by default, only override for ANDI and ORI
		id_ex.immediate = s_imm;
		// Write back to rt
		id_ex.ex_mem.mem_wb.reg = rt;

		switch (opc) {
			case MIPS_OPC_ADDIU: id_ex.alu_op = ALU_OP_ADD; break;

			case MIPS_OPC_ANDI:
				id_ex.alu_op    = ALU_OP_AND;
				id_ex.immediate = z_imm;
				break;

			case MIPS_OPC_ORI: {
				id_ex.alu_op    = ALU_OP_OR;
				id_ex.immediate = z_imm;
			} break;

			case MIPS_OPC_LW: {
				id_ex.alu_op             = ALU_OP_ADD;
				id_ex.ex_mem.access_type = MEM_ACCESS_READ_SIGNED;
				id_ex.ex_mem.bytes       = 4;
			} break;

			default: log_dbgi("Unknown opcode\n"); break;
		}
	}

	return id_ex;
}
