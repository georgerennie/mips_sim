#include "common/esbmc_util.h"
#include "test_common/pipeline_test.h"

static void esbmc_id_r_test() {
	const mips_funct_t   funct   = EXTRACT_BITS(5, 0, nondet_u8());
	const mips_reg_idx_t rs      = nondet_reg();
	const uint32_t       data_rs = nondet_u32();
	const mips_reg_idx_t rt      = nondet_reg();
	const uint32_t       data_rt = nondet_u32();
	const mips_reg_idx_t rd      = nondet_reg();

	alu_op_t         alu_op    = ALU_OP_NOP;
	mips_exception_t exception = {.raised = false};
	switch (funct) {
		case MIPS_FUNCT_NOP: alu_op = ALU_OP_NOP; break;
		case MIPS_FUNCT_ADDU: alu_op = ALU_OP_ADD; break;
		case MIPS_FUNCT_AND: alu_op = ALU_OP_AND; break;
		case MIPS_FUNCT_OR: alu_op = ALU_OP_OR; break;
		case MIPS_FUNCT_BREAK:
			exception.raised = true;
			exception.cause  = MIPS_EXCP_BREAK;
			break;
		default:
			exception.raised = true;
			exception.cause  = MIPS_EXCP_RI;
			break;
	}

	id_test_r_type(funct, rs, data_rs, rt, data_rt, rd, alu_op, exception);
}

static void esbmc_id_i_alu_test() {
	const mips_opcode_t  opc       = EXTRACT_BITS(5, 0, nondet_u8());
	const mips_reg_idx_t rs        = nondet_reg();
	const uint32_t       data_rs   = nondet_u32();
	const uint16_t       immediate = nondet_u16();
	const mips_reg_idx_t rt        = nondet_reg();

	esbmc_assume(
	    (opc != MIPS_OPC_R_FMT) && (opc != MIPS_OPC_J) && (opc != MIPS_OPC_BEQ) &&
	    (opc != MIPS_OPC_BNE) && (opc != MIPS_OPC_SH) && (opc != MIPS_OPC_LHU));

	alu_op_t         alu_op    = ALU_OP_NOP;
	mips_exception_t exception = {.raised = false};
	switch (opc) {
		case MIPS_OPC_ADDIU: alu_op = ALU_OP_ADD; break;
		case MIPS_OPC_ANDI: alu_op = ALU_OP_AND; break;
		case MIPS_OPC_ORI: alu_op = ALU_OP_OR; break;
		default:
			exception.raised = true;
			exception.cause  = MIPS_EXCP_RI;
			break;
	}

	id_test_i_type_alu(opc, rs, data_rs, immediate, rt, alu_op, exception);
}

static void esbmc_id_i_branch_test() {
	const mips_opcode_t  opc         = EXTRACT_BITS(5, 0, nondet_u8());
	const mips_reg_idx_t rs          = nondet_reg();
	const uint32_t       data_rs     = nondet_u32();
	const mips_reg_idx_t rt          = nondet_reg();
	const uint32_t       data_rt     = nondet_u32();
	const int16_t        branch_addr = (int16_t) (nondet_u16() & 0xFFFC);

	esbmc_assume((opc == MIPS_OPC_BEQ) || (opc == MIPS_OPC_BNE));
	const bool data_eq       = data_rs == data_rt;
	const bool should_branch = (opc == MIPS_OPC_BEQ) ? data_eq : !data_eq;

	id_test_i_type_branch(opc, rs, data_rs, rt, data_rt, branch_addr, should_branch);
}

static void esbmc_id_j_test() {
	const mips_j_address_t address = EXTRACT_BITS(25, 0, nondet_u32());
	id_test_j_type(address);
}

static void esbmc_id_i_mem_test() {
	const mips_opcode_t  opc       = EXTRACT_BITS(5, 0, nondet_u8());
	const mips_reg_idx_t rs        = nondet_reg();
	const uint32_t       data_rs   = nondet_u32();
	const mips_reg_idx_t rt        = nondet_reg();
	const uint32_t       data_rt   = nondet_u32();
	const int16_t        immediate = nondet_i16();

	esbmc_assume((opc == MIPS_OPC_SH) || (opc == MIPS_OPC_LHU));
	id_test_i_type_mem(opc, rs, data_rs, rt, data_rt, immediate);
}

static void esbmc_ex_test() {
	const uint32_t data_rs = nondet_u32();
	const uint32_t data_rt = nondet_u32();
	const uint32_t imm     = nondet_u32();
	const alu_op_t op      = (alu_op_t) nondet_u32();
	esbmc_assume(op >= 0 && op < ALU_OP_MAX);

	const bool     use_imm = nondet_bool();
	const uint32_t b       = use_imm ? imm : data_rt;
	uint32_t       res     = data_rs;
	switch (op) {
		case ALU_OP_ADD: res += b; break;
		case ALU_OP_AND: res &= b; break;
		case ALU_OP_OR: res |= b; break;
		default: break;
	}

	if (!use_imm) {
		ex_test_reg_no_fwd(data_rs, data_rt, op, res);
	} else {
		ex_test_imm_no_fwd(data_rs, imm, op, res);
	}
}

static void esbmc_mem_test() {
	const uint32_t mem_size = nondet_u32();
	const uint16_t value    = nondet_u16();
	const uint32_t address  = nondet_u32();

	mem_test_read(mem_size, value, address);
	mem_test_write(mem_size, value, address);
}

static void esbmc_wb_test() {
	const uint32_t       value = nondet_u32();
	const mips_reg_idx_t reg   = nondet_reg();
	wb_test(value, reg);
}

int main() {
	esbmc_id_r_test();
	esbmc_id_i_alu_test();
	esbmc_id_i_branch_test();
	esbmc_id_i_mem_test();
	esbmc_id_j_test();

	esbmc_ex_test();
	esbmc_mem_test();
	esbmc_wb_test();
}
