#include "pipeline_test.h"
#include "common/esbmc_util.h"
#include "common/log.h"

// -------- Instruction decode tests -------------------------------------------

static mips_pipeline_regs_t id_init_test_regs(uint32_t instruction) {
	mips_pipeline_regs_t regs;
	regs.if_id.metadata.instruction      = instruction;
	regs.if_id.metadata.address          = 0x40004000;
	regs.if_id.metadata.exception.raised = false;
	return regs;
}

static mips_state_t id_init_test_state(
    mips_reg_idx_t rs, uint32_t data_rs, mips_reg_idx_t rt, uint32_t data_rt) {
	mips_state_t state;
	if (rs == rt && rs != 0) { esbmc_assume(data_rs == data_rt); }
	if (rs == 0) { esbmc_assume(data_rs == 0); }
	if (rt == 0) { esbmc_assume(data_rt == 0); }
	state.gpr[rs] = data_rs;
	state.gpr[rt] = data_rt;
	return state;
}

void id_test_r_type(
    mips_funct_t funct, mips_reg_idx_t rs, uint32_t data_rs, mips_reg_idx_t rt, uint32_t data_rt,
    mips_reg_idx_t rd, alu_op_t expected_alu_op, mips_exception_t expected_exception) {
	const uint32_t instruction =
	    (uint32_t) INSERT_BITS(25, 21, rs) | (uint32_t) INSERT_BITS(20, 16, rt) |
	    (uint32_t) INSERT_BITS(15, 11, rd) | (uint32_t) INSERT_BITS(5, 0, funct);
	const mips_pipeline_regs_t regs  = id_init_test_regs(instruction);
	const mips_state_t         state = id_init_test_state(rs, data_rs, rt, data_rt);
	const id_ex_reg_t          id_ex = instruction_decode(&regs, &state);

	log_assert_eqi(id_ex.data_rs, (rs == 0) ? 0 : data_rs);
	log_assert_eqi(id_ex.data_rt, (rt == 0) ? 0 : data_rt);
	log_assert_eqi(id_ex.reg_rs, rs);
	log_assert_eqi(id_ex.reg_rt, rt);

	log_assert_cond_maps(id_ex.alu_op, ==, expected_alu_op, alu_op_to_str);
	log_assert_cond_maps(id_ex.alu_b_src, ==, ALU_SRC_DATA_B, alu_src_to_str);

	log_assert_eqi(id_ex.eval_branch, false);
	log_assert_eqi(id_ex.branch, false);
	log_assert_cond_maps(id_ex.ex_mem.access_type, ==, MEM_ACCESS_NONE, mem_access_to_str);
	log_assert_eqi(id_ex.ex_mem.mem_wb.metadata.instruction, regs.if_id.metadata.instruction);
	log_assert_eqi(id_ex.ex_mem.mem_wb.metadata.address, regs.if_id.metadata.address);

	if (expected_exception.raised) {
		log_assert_eqi(id_ex.ex_mem.mem_wb.metadata.exception.raised, true);
		log_assert_cond_maps(
		    id_ex.ex_mem.mem_wb.metadata.exception.cause, ==, expected_exception.cause,
		    mips_exception_name);

		log_assert_eqi(id_ex.ex_mem.mem_wb.reg, 0);
		return;
	}

	// No exception occured
	log_assert_eqi(id_ex.ex_mem.mem_wb.metadata.exception.raised, false);
	if (id_ex.alu_op == ALU_OP_NOP) {
		log_assert_eqi(id_ex.ex_mem.mem_wb.reg, 0);
	} else {
		log_assert_eqi(id_ex.ex_mem.mem_wb.reg, rd);
	}
}

void id_test_i_type_alu(
    mips_opcode_t opc, mips_reg_idx_t rs, uint32_t data_rs, uint16_t immediate, mips_reg_idx_t rt,
    alu_op_t expected_alu_op, mips_exception_t expected_exception) {
	const uint32_t instruction =
	    (uint32_t) INSERT_BITS(31, 26, opc) | (uint32_t) INSERT_BITS(25, 21, rs) |
	    (uint32_t) INSERT_BITS(20, 16, rt) | (uint32_t) INSERT_BITS(15, 0, immediate);
	const mips_pipeline_regs_t regs  = id_init_test_regs(instruction);
	const mips_state_t         state = id_init_test_state(rs, data_rs, 0, 0);
	const id_ex_reg_t          id_ex = instruction_decode(&regs, &state);

	log_assert_eqi(id_ex.data_rs, data_rs);
	log_assert_eqi(id_ex.reg_rs, rs);

	log_assert_cond_maps(id_ex.alu_op, ==, expected_alu_op, alu_op_to_str);
	log_assert_cond_maps(id_ex.alu_b_src, ==, ALU_SRC_IMM, alu_src_to_str);

	if (id_ex.alu_op == ALU_OP_AND || id_ex.alu_op == ALU_OP_OR) {
		log_assert_eqi(id_ex.immediate, immediate);
	} else {
		log_assert_eqi(id_ex.immediate, (uint32_t) (int16_t) immediate);
	}

	log_assert_eqi(id_ex.eval_branch, false);
	log_assert_eqi(id_ex.branch, false);
	log_assert_eqi(id_ex.ex_mem.mem_wb.metadata.instruction, regs.if_id.metadata.instruction);
	log_assert_eqi(id_ex.ex_mem.mem_wb.metadata.address, regs.if_id.metadata.address);

	log_assert_cond_maps(id_ex.ex_mem.access_type, ==, MEM_ACCESS_NONE, mem_access_to_str);

	if (expected_exception.raised) {
		log_assert_eqi(id_ex.ex_mem.mem_wb.metadata.exception.raised, true);
		log_assert_cond_maps(
		    id_ex.ex_mem.mem_wb.metadata.exception.cause, ==, expected_exception.cause,
		    mips_exception_name);

		log_assert_eqi(id_ex.ex_mem.mem_wb.reg, 0);
		return;
	}

	// No exception occured
	log_assert_eqi(id_ex.ex_mem.mem_wb.metadata.exception.raised, false);
	log_assert_cond_maps(id_ex.alu_op, !=, ALU_OP_NOP, alu_op_to_str);
	log_assert_eqi(id_ex.ex_mem.mem_wb.reg, rt);
}

void id_test_i_type_branch(
    mips_opcode_t opc, mips_reg_idx_t rs, uint32_t data_rs, mips_reg_idx_t rt, uint32_t data_rt,
    int16_t branch_addr, bool should_branch) {
	// Don't test forwarding, that is tested separately with integration tests
	const uint32_t instruction = (uint32_t) INSERT_BITS(31, 26, opc) |
	                             (uint32_t) INSERT_BITS(25, 21, rs) |
	                             (uint32_t) INSERT_BITS(20, 16, rt) |
	                             (uint32_t) INSERT_BITS(15, 0, (uint32_t) (branch_addr >> 2));
	mips_pipeline_regs_t regs = id_init_test_regs(instruction);
	regs.ex_mem.mem_wb        = (mem_wb_reg_t){.reg = 0};
	regs.mem_wb               = (mem_wb_reg_t){.reg = 0};
	const mips_state_t state  = id_init_test_state(rs, data_rs, rt, data_rt);
	const id_ex_reg_t  id_ex  = instruction_decode(&regs, &state);

	log_assert_eqi(id_ex.branch, should_branch);
	log_assert_eqi(id_ex.eval_branch, true);
	log_assert_eqi(
	    id_ex.branch_address, id_ex.ex_mem.mem_wb.metadata.address + 4 + (uint32_t) branch_addr);

	log_assert_cond_maps(id_ex.alu_op, ==, ALU_OP_NOP, alu_op_to_str);
	log_assert_eqi(id_ex.ex_mem.mem_wb.reg, 0);
	log_assert_cond_maps(id_ex.ex_mem.access_type, ==, MEM_ACCESS_NONE, mem_access_to_str);

	log_assert_eqi(id_ex.ex_mem.mem_wb.metadata.exception.raised, false);
}

void id_test_i_type_mem(
    mips_opcode_t opc, mips_reg_idx_t rs, uint32_t data_rs, mips_reg_idx_t rt, uint32_t data_rt,
    int16_t immediate) {
	const uint32_t instruction =
	    (uint32_t) INSERT_BITS(31, 26, opc) | (uint32_t) INSERT_BITS(25, 21, rs) |
	    (uint32_t) INSERT_BITS(20, 16, rt) | (uint32_t) INSERT_BITS(15, 0, (uint32_t) immediate);
	const mips_pipeline_regs_t regs  = id_init_test_regs(instruction);
	const mips_state_t         state = id_init_test_state(rs, data_rs, rt, data_rt);
	const id_ex_reg_t          id_ex = instruction_decode(&regs, &state);

	log_assert_eqi(id_ex.data_rs, data_rs);
	log_assert_eqi(id_ex.reg_rs, rs);
	log_assert_eqi(id_ex.immediate, (uint32_t) immediate);
	log_assert_cond_maps(id_ex.alu_op, ==, ALU_OP_ADD, alu_op_to_str);
	log_assert_cond_maps(id_ex.alu_b_src, ==, ALU_SRC_IMM, alu_src_to_str);

	log_assert_eqi(id_ex.ex_mem.bytes, 2);
	if (opc == MIPS_OPC_LHU) {
		log_assert_cond_maps(
		    id_ex.ex_mem.access_type, ==, MEM_ACCESS_READ_UNSIGNED, mem_access_to_str);
		log_assert_eqi(id_ex.ex_mem.mem_wb.reg, rt);
	} else {
		log_assert_cond_maps(id_ex.ex_mem.access_type, ==, MEM_ACCESS_WRITE, mem_access_to_str);
	}

	log_assert_eqi(id_ex.eval_branch, false);
	log_assert_eqi(id_ex.branch, false);
	log_assert_eqi(id_ex.ex_mem.mem_wb.metadata.exception.raised, false);
}

void id_test_j_type(mips_j_address_t address) {
	const uint32_t instruction = (uint32_t) INSERT_BITS(31, 26, MIPS_OPC_J) |
	                             (uint32_t) INSERT_BITS(25, 0, (uint32_t) address);
	const mips_pipeline_regs_t regs = id_init_test_regs(instruction);
	const mips_state_t         state;
	const id_ex_reg_t          id_ex = instruction_decode(&regs, &state);

	log_assert_eqi(id_ex.branch, true);
	log_assert_eqi(id_ex.eval_branch, false);
	// TODO: Is this right?
	log_assert_eqi(
	    id_ex.branch_address,
	    ((regs.if_id.metadata.address + 4) & 0xF0000000) | ((uint32_t) address << 2));

	log_assert_cond_maps(id_ex.ex_mem.access_type, ==, MEM_ACCESS_NONE, mem_access_to_str);
	log_assert_eqi(id_ex.ex_mem.mem_wb.reg, 0);
	log_assert_eqi(id_ex.ex_mem.mem_wb.metadata.exception.raised, false);
}

// -------- Execute tests ------------------------------------------------------
// These don't test forwarding, that is tested separately with integration tests
// as it is a pain to unit test

void ex_test_reg_no_fwd(uint32_t data_rs, uint32_t data_rt, alu_op_t op, uint32_t expected_result) {
	const mips_pipeline_regs_t regs = {
	    .id_ex =
	        {
	            .data_rs   = data_rs,
	            .data_rt   = data_rt,
	            .alu_b_src = ALU_SRC_DATA_B,
	            .reg_rs    = 0,
	            .reg_rt    = 0,
	            .alu_op    = op,
	        },
	};
	const ex_mem_reg_t ex_mem = execute(&regs);
	log_assert_eqi(ex_mem.mem_wb.result, expected_result);
}

void ex_test_imm_no_fwd(uint32_t data_rs, uint32_t imm, alu_op_t op, uint32_t expected_result) {
	const mips_pipeline_regs_t regs = {
	    .id_ex =
	        {
	            .data_rs   = data_rs,
	            .immediate = imm,
	            .alu_b_src = ALU_SRC_IMM,
	            .reg_rs    = 0,
	            .alu_op    = op,
	        },
	};
	const ex_mem_reg_t ex_mem = execute(&regs);
	log_assert_eqi(ex_mem.mem_wb.result, expected_result);
}

// -------- Memory tests -------------------------------------------------------

static bool mem_expect_exception(uint32_t mem_size, uint32_t address, uint8_t alignment) {
	if (address % alignment != 0) { return true; }
	if ((uint64_t) address + alignment > mem_size) { return true; }
	return false;
}

void mem_test_read(uint32_t mem_size, uint16_t value, uint32_t address) {
	uint8_t data_mem[mem_size];

	// Little endian
	if ((uint64_t) address + 1 < mem_size) {
		// Only tests half word reads
		data_mem[address]     = EXTRACT_BITS(15, 8, value);
		data_mem[address + 1] = EXTRACT_BITS(7, 0, value);
	}

	const ex_mem_reg_t ex_mem = {
	    .access_type   = MEM_ACCESS_READ_UNSIGNED,
	    .mem_wb.result = address,
	    .bytes         = 2,
	};
	const mem_wb_reg_t mem_wb = memory(&ex_mem, (span_t){.data = data_mem, .size = mem_size});

	if (mem_expect_exception(mem_size, address, 2)) {
		log_assert_eqi(mem_wb.metadata.exception.raised, true);
		log_assert_eqi(mem_wb.metadata.exception.cause, MIPS_EXCP_ADDRL);
		log_assert_eqi(mem_wb.metadata.exception.bad_v_addr, address);
		log_assert_eqi(mem_wb.reg, 0);
		return;
	}

	log_assert_eqi(mem_wb.metadata.exception.raised, false);
	log_assert_eqi(mem_wb.result, value);
}

void mem_test_write(uint32_t mem_size, uint16_t value, uint32_t address) {
	uint8_t            data_mem[mem_size];
	const ex_mem_reg_t ex_mem = {
	    .access_type   = MEM_ACCESS_WRITE,
	    .mem_wb.result = address,
	    .data_rt       = value,
	    .bytes         = 2,
	};
	const mem_wb_reg_t mem_wb = memory(&ex_mem, (span_t){.data = data_mem, .size = mem_size});

	log_assert_eqi(mem_wb.reg, 0);
	if (mem_expect_exception(mem_size, address, 2)) {
		log_assert_eqi(mem_wb.metadata.exception.raised, true);
		log_assert_eqi(mem_wb.metadata.exception.cause, MIPS_EXCP_ADDRS);
		log_assert_eqi(mem_wb.metadata.exception.bad_v_addr, address);
		return;
	}

	log_assert_eqi(mem_wb.metadata.exception.raised, false);
	log_assert_eqi(data_mem[address], EXTRACT_BITS(15, 8, value));
	log_assert_eqi(data_mem[address + 1], EXTRACT_BITS(7, 0, value));
}

// -------- Writeback tests ----------------------------------------------------

void wb_test(uint32_t value, mips_reg_idx_t reg) {
	const mem_wb_reg_t mem_wb = {.reg = reg, .result = value};
	mips_state_t       state  = {.gpr[0] = 0};

	writeback(&mem_wb, &state);
	log_assert_eqi(state.gpr[reg], reg == 0 ? 0 : value);
}
