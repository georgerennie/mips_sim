#include "instruction_decoder.h"
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include "util.h"

static inline uint32_t sign_extend(uint16_t val) { return (uint32_t) (int16_t) val; }

decode_bundle_t decode_instruction(const mips_state_t* state) {
	mips_instr_t instr = parse_instruction(state);

	decode_bundle_t bundle = {
	    .format    = instr.format,
	    .alu_op    = ALU_OP_NOP,
	    .alu_arg_a = 0,
	    .alu_arg_b = 0,

	    .mem_access_type = MEM_ACCESS_NONE,
	    .mem_bytes       = 0,
	    .writeback_reg   = 0,
	};

	if (bundle.format == MIPS_INSTR_FORMAT_R) {
		switch (instr.r_data.funct) {
			case MIPS_FUNCT_ADD:
			case MIPS_FUNCT_ADDU: bundle.alu_op = ALU_OP_ADD; break;

			case MIPS_FUNCT_SUB:
			case MIPS_FUNCT_SUBU: bundle.alu_op = ALU_OP_SUB; break;

			case MIPS_FUNCT_AND: bundle.alu_op = ALU_OP_AND; break;
			case MIPS_FUNCT_OR: bundle.alu_op = ALU_OP_OR; break;
			case MIPS_FUNCT_XOR: bundle.alu_op = ALU_OP_XOR; break;
			case MIPS_FUNCT_NOR: bundle.alu_op = ALU_OP_NOR; break;

			default: bundle.alu_op = ALU_OP_NOP; break;
		}

		bundle.alu_arg_a     = gpr_read(state, instr.r_data.rs);
		bundle.alu_arg_b     = gpr_read(state, instr.r_data.rt);
		bundle.writeback_reg = instr.r_data.rd;

	} else if (bundle.format == MIPS_INSTR_FORMAT_I) {
		bundle.alu_arg_a = gpr_read(state, instr.i_data.rs);
		// Sign extend by default, only override for ANDI and ORI
		bundle.alu_arg_b     = sign_extend(instr.i_data.immediate);
		bundle.writeback_reg = instr.i_data.rt;

		switch (instr.opcode) {
			case MIPS_OPC_ADDI:
			case MIPS_OPC_ADDIU: bundle.alu_op = ALU_OP_ADD; break;

			case MIPS_OPC_ANDI: {
				bundle.alu_op    = ALU_OP_AND;
				bundle.alu_arg_b = instr.i_data.immediate;
			} break;
			case MIPS_OPC_ORI: {
				bundle.alu_op    = ALU_OP_OR;
				bundle.alu_arg_b = instr.i_data.immediate;
			} break;

			case MIPS_OPC_LB:
			case MIPS_OPC_LBU:
			case MIPS_OPC_LHU:
			case MIPS_OPC_LW: {
				bundle.alu_op          = ALU_OP_ADD;
				bundle.mem_access_type = (instr.opcode == MIPS_OPC_LB) ? MEM_ACCESS_READ_SIGNED
				                                                       : MEM_ACCESS_READ_UNSIGNED;
				bundle.mem_bytes       = (instr.opcode == MIPS_OPC_LW)    ? 4
				                         : (instr.opcode == MIPS_OPC_LHU) ? 2
				                                                          : 1;
			} break;

			case MIPS_OPC_LUI: {
				bundle.alu_op    = ALU_OP_NOP;
				bundle.alu_arg_a = (uint32_t) instr.i_data.immediate << 16;
			} break;

			default: bundle.alu_op = ALU_OP_NOP; break;
		}
	} else if (bundle.format == MIPS_INSTR_FORMAT_J) {
	} else if (bundle.format == MIPS_INSTR_FORMAT_UNKNOWN) {
	} else {
		assert(0);
	}

	return bundle;
}

mips_instr_t parse_instruction(const mips_state_t* state) {
	const uint32_t instr = state->ir;
	mips_instr_t   decoded;
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
