#include "instruction_decoder.h"
#include "util/log.h"
#include "util/util.h"

static inline uint32_t sign_extend(uint16_t val) { return (uint32_t) (int16_t) val; }

static inline alu_fwd_src_t get_fwd(const mips_core_t* core, uint32_t reg) {
	// TODO: Should this obey stalls?
	if (reg == core->exec_bundle.mem.wb.reg) {
		return ALU_FWD_SRC_EXEC;
	} else if (reg == core->mem_bundle.wb.reg) {
		return ALU_FWD_SRC_MEM;
	} else {
		return ALU_FWD_SRC_NONE;
	}
}

static inline void log_dbg_opc(const char* opcode) { log_dbgi("Decoded %s\n", opcode); }

decode_result_t decode_instruction(const mips_core_t* core, uint32_t instruction) {
	mips_instr_t instr = parse_instruction(instruction);

	// Initialise values to pass down the pipeline
	decode_result_t ret;
	ret.trap = MIPS_TRAP_NONE;
	execute_bundle_init(&ret.exec);

	if (instr.format == MIPS_INSTR_FORMAT_R) {
		switch (instr.r_data.funct) {
			case MIPS_FUNCT_ADDU:
				log_dbg_opc("add");
				ret.exec.op = ALU_OP_ADD;
				break;

			case MIPS_FUNCT_AND:
				log_dbg_opc("and");
				ret.exec.op = ALU_OP_AND;
				break;
			case MIPS_FUNCT_OR:
				log_dbg_opc("or");
				ret.exec.op = ALU_OP_OR;
				break;

			default:
				log_dbg_opc("unknown r type");
				ret.trap |= MIPS_TRAP_UNKNOWN_INSTR;
				break;
		}

		ret.exec.arg_a      = gpr_read(&core->state, instr.r_data.rs);
		ret.exec.fwd_a      = get_fwd(core, instr.r_data.rs);
		ret.exec.arg_b      = gpr_read(&core->state, instr.r_data.rt);
		ret.exec.fwd_b      = get_fwd(core, instr.r_data.rt);
		ret.exec.mem.wb.reg = instr.r_data.rd;

	} else if (instr.format == MIPS_INSTR_FORMAT_I) {
		ret.exec.arg_a = gpr_read(&core->state, instr.i_data.rs);
		ret.exec.fwd_a = get_fwd(core, instr.r_data.rs);
		// Sign extend by default, only override for ANDI and ORI
		ret.exec.arg_b      = sign_extend(instr.i_data.immediate);
		ret.exec.mem.wb.reg = instr.i_data.rt;

		switch (instr.opcode) {
			case MIPS_OPC_ADDIU:
				log_dbg_opc("addiu");
				ret.exec.op = ALU_OP_ADD;
				break;

			case MIPS_OPC_ANDI: {
				log_dbg_opc("andi");
				ret.exec.op    = ALU_OP_AND;
				ret.exec.arg_b = instr.i_data.immediate;
			} break;
			case MIPS_OPC_ORI: {
				log_dbg_opc("ori");
				ret.exec.op    = ALU_OP_OR;
				ret.exec.arg_b = instr.i_data.immediate;
			} break;

			case MIPS_OPC_LW: {
				log_dbg_opc("lw");
				ret.exec.op              = ALU_OP_ADD;
				ret.exec.mem.access_type = MEM_ACCESS_READ_SIGNED;
				ret.exec.mem.bytes       = 4;
			} break;

			default: ret.trap |= MIPS_TRAP_UNKNOWN_INSTR; break;
		}
	} else if (instr.format == MIPS_INSTR_FORMAT_J) {
		log_dbg_opc("J format instruction. Ignored");
	} else if (instr.format == MIPS_INSTR_FORMAT_UNKNOWN) {
		log_dbg_opc("unknown opcode");
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

		case MIPS_OPC_ADDIU:
		case MIPS_OPC_ANDI:
		case MIPS_OPC_ORI:
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
