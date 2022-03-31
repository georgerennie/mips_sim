#include "instruction_decoder.h"
#include "util/log.h"
#include "util/util.h"

static inline uint32_t sign_extend(uint16_t val) { return (uint32_t) (int16_t) val; }

decode_result_t decode_instruction(const mips_state_t* state, uint32_t instruction) {
	mips_instr_t instr = parse_instruction(instruction);

	// Initialise values to pass down the pipeline
	decode_result_t ret;
	ret.trap = MIPS_TRAP_NONE;
	execute_bundle_init(&ret.exec);

	if (instr.format == MIPS_INSTR_FORMAT_R) {
		switch (instr.r_data.funct) {
			case MIPS_FUNCT_ADD:
			case MIPS_FUNCT_ADDU: ret.exec.op = ALU_OP_ADD; break;

			case MIPS_FUNCT_SUB:
			case MIPS_FUNCT_SUBU: ret.exec.op = ALU_OP_SUB; break;

			case MIPS_FUNCT_AND: ret.exec.op = ALU_OP_AND; break;
			case MIPS_FUNCT_OR: ret.exec.op = ALU_OP_OR; break;
			case MIPS_FUNCT_XOR: ret.exec.op = ALU_OP_XOR; break;
			case MIPS_FUNCT_NOR: ret.exec.op = ALU_OP_NOR; break;

			default: ret.trap |= MIPS_TRAP_UNKNOWN_INSTR; break;
		}

		ret.exec.arg_a      = gpr_read(state, instr.r_data.rs);
		ret.exec.arg_b      = gpr_read(state, instr.r_data.rt);
		ret.exec.mem.wb.reg = instr.r_data.rd;

	} else if (instr.format == MIPS_INSTR_FORMAT_I) {
		ret.exec.arg_a = gpr_read(state, instr.i_data.rs);
		// Sign extend by default, only override for ANDI and ORI
		ret.exec.arg_b      = sign_extend(instr.i_data.immediate);
		ret.exec.mem.wb.reg = instr.i_data.rt;

		switch (instr.opcode) {
			case MIPS_OPC_ADDI:
			case MIPS_OPC_ADDIU: ret.exec.op = ALU_OP_ADD; break;

			case MIPS_OPC_ANDI: {
				ret.exec.op    = ALU_OP_AND;
				ret.exec.arg_b = instr.i_data.immediate;
			} break;
			case MIPS_OPC_ORI: {
				ret.exec.op    = ALU_OP_OR;
				ret.exec.arg_b = instr.i_data.immediate;
			} break;

			case MIPS_OPC_LB:
			case MIPS_OPC_LBU:
			case MIPS_OPC_LHU:
			case MIPS_OPC_LW: {
				ret.exec.op              = ALU_OP_ADD;
				ret.exec.mem.access_type = (instr.opcode == MIPS_OPC_LB) ? MEM_ACCESS_READ_SIGNED
				                                                         : MEM_ACCESS_READ_UNSIGNED;
				ret.exec.mem.bytes       = (instr.opcode == MIPS_OPC_LW)    ? 4
				                           : (instr.opcode == MIPS_OPC_LHU) ? 2
				                                                            : 1;
			} break;

			case MIPS_OPC_LUI: {
				ret.exec.op    = ALU_OP_NOP;
				ret.exec.arg_a = (uint32_t) instr.i_data.immediate << 16;
			} break;

			default: ret.trap |= MIPS_TRAP_UNKNOWN_INSTR; break;
		}
	} else if (instr.format == MIPS_INSTR_FORMAT_J) {
	} else if (instr.format == MIPS_INSTR_FORMAT_UNKNOWN) {
		ret.trap |= MIPS_TRAP_UNKNOWN_INSTR;
	} else {
		log_assert_fail("Unrecognised instruction format %d\n", instr.format);
	}

	return ret;
}

mips_instr_t parse_instruction(uint32_t instr) {
	mips_instr_t decoded;
	decoded.opcode = EXTRACT_BITS(31, 26, instr);

	switch (decoded.opcode) {
		case MIPS_OPC_R_FMT: {
			decoded.format       = MIPS_INSTR_FORMAT_R;
			decoded.r_data.rs    = EXTRACT_BITS(25, 21, instr);
			decoded.r_data.rt    = EXTRACT_BITS(20, 16, instr);
			decoded.r_data.rd    = EXTRACT_BITS(15, 11, instr);
			decoded.r_data.shamt = EXTRACT_BITS(10, 6, instr);
			decoded.r_data.funct = EXTRACT_BITS(5, 0, instr);
		} break;

		case MIPS_OPC_ADDI:
		case MIPS_OPC_ADDIU:
		case MIPS_OPC_ANDI:
		case MIPS_OPC_ORI:
		case MIPS_OPC_LUI:
		case MIPS_OPC_LB:
		case MIPS_OPC_LBU:
		case MIPS_OPC_LHU:
		case MIPS_OPC_LW: {
			decoded.format           = MIPS_INSTR_FORMAT_I;
			decoded.i_data.rs        = EXTRACT_BITS(25, 21, instr);
			decoded.i_data.rt        = EXTRACT_BITS(20, 16, instr);
			decoded.i_data.immediate = EXTRACT_BITS(15, 0, instr);
		} break;

		case MIPS_OPC_J: {
			decoded.format         = MIPS_INSTR_FORMAT_J;
			decoded.j_data.address = EXTRACT_BITS(25, 0, instr);
		} break;

		// TODO: Add printing for this assertion
		default: decoded.format = MIPS_INSTR_FORMAT_UNKNOWN; break;
	}

	return decoded;
}
