#include "core.h"
#include "forwarding_unit.h"
#include "instruction.h"
#include "util/log.h"
#include "util/util.h"

id_ex_reg_t instruction_decode(const mips_pipeline_regs_t* regs, const mips_state_t* arch_state) {
	const uint32_t      instr = regs->if_id.instruction;
	const mips_opcode_t opc   = EXTRACT_BITS(31, 26, instr);
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

	// J type
	const uint32_t jump_address = EXTRACT_BITS(25, 0, instr);

	// Initialise values to pass down the pipeline
	id_ex_reg_t id_ex;
	id_ex_reg_init(&id_ex);

	// Setup instruction metadata
	id_ex.ex_mem.mem_wb.metadata.instruction = regs->if_id.instruction;
	id_ex.ex_mem.mem_wb.metadata.address     = regs->if_id.address;

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
			case MIPS_FUNCT_NOP: id_ex.ex_mem.mem_wb.reg = 0; break;

			case MIPS_FUNCT_ADDU: id_ex.alu_op = ALU_OP_ADD; break;
			case MIPS_FUNCT_AND: id_ex.alu_op = ALU_OP_AND; break;
			case MIPS_FUNCT_OR: id_ex.alu_op = ALU_OP_OR; break;

			default: log_dbgi("Unrecognised funct 0x%02X for r type instruction\n", funct); break;
		}
	}

	else if (opc == MIPS_OPC_J) {
		id_ex.branch         = true;
		id_ex.branch_address = ((regs->if_id.address + 4) & 0xF0000000) | (jump_address << 2);
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

			case MIPS_OPC_LHU: {
				id_ex.alu_op             = ALU_OP_ADD;
				id_ex.ex_mem.access_type = MEM_ACCESS_READ_UNSIGNED;
				id_ex.ex_mem.bytes       = 2;
			} break;

			case MIPS_OPC_SH: {
				id_ex.alu_op             = ALU_OP_ADD;
				id_ex.ex_mem.access_type = MEM_ACCESS_WRITE;
				id_ex.ex_mem.bytes       = 2;
				id_ex.ex_mem.mem_wb.reg  = 0;
			} break;

			case MIPS_OPC_BEQ:
			case MIPS_OPC_BNE: {
				id_ex.ex_mem.mem_wb.reg   = 0;
				id_ex.eval_branch         = true;
				const uint32_t rs_fwd     = get_fwd_value(regs, rs, id_ex.data_rs);
				const uint32_t rt_fwd     = get_fwd_value(regs, rt, id_ex.data_rt);
				const bool     regs_equal = rs_fwd == rt_fwd;
				id_ex.branch              = (opc == MIPS_OPC_BEQ) ? regs_equal : !regs_equal;
				id_ex.branch_address      = regs->if_id.address + 4 + (s_imm << 2);
			} break;

			default: log_dbgi("Unknown opcode\n"); break;
		}
	}

	return id_ex;
}

void if_id_reg_init(if_id_reg_t* if_id) {
	if_id->instruction = 0x00000000;
	if_id->address     = 0x00000000;
}
