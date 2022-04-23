#include "test.h"
#include "test_common/pipeline_test.h"

// -------- Instruction decode R type tests ------------------------------------
#define TEST_R_NO_EXCP(NAME, FUNCT, RS, DATA_RS, RT, DATA_RT, RD, EXP_ALU_OP)  \
	DEFINE_TEST(id_r_no_excp_##NAME) {                                         \
		mips_exception_t excp = {.raised = false};                             \
		id_test_r_type(FUNCT, RS, DATA_RS, RT, DATA_RT, RD, EXP_ALU_OP, excp); \
	}

#define TEST_R_EXCP(NAME, FUNCT, RS, DATA_RS, RT, DATA_RT, RD, EXP_ALU_OP, EXCP_CAUSE) \
	DEFINE_TEST(id_r_excp_##NAME) {                                                    \
		mips_exception_t excp = {.raised = true, .cause = EXCP_CAUSE};                 \
		id_test_r_type(FUNCT, RS, DATA_RS, RT, DATA_RT, RD, EXP_ALU_OP, excp);         \
	}

// name, funct, rs, *rs, rt, *rt, rd, expected alu op
TEST_R_NO_EXCP(addu, MIPS_FUNCT_ADDU, 1, 130, 2, 210, 3, ALU_OP_ADD)
TEST_R_NO_EXCP(addu_rs_rt_zero, MIPS_FUNCT_ADDU, 0, 0, 0, 0, 3, ALU_OP_ADD)
TEST_R_NO_EXCP(addu_rd_zero, MIPS_FUNCT_ADDU, 31, 51, 25, 61, 0, ALU_OP_ADD)
TEST_R_NO_EXCP(and, MIPS_FUNCT_AND, 20, 1920, 15, 1049, 15, ALU_OP_AND)
TEST_R_NO_EXCP(or, MIPS_FUNCT_OR, 20, 1920, 15, 1049, 15, ALU_OP_OR)

// name, funct, rs, *rs, rt, *rt, rd, expected alu op, expected exception cause
TEST_R_EXCP(break, MIPS_FUNCT_BREAK, 20, 1920, 15, 1049, 15, ALU_OP_NOP, MIPS_EXCP_BREAK)
TEST_R_EXCP(invalid_1, 0x78, 20, 1920, 15, 1049, 15, ALU_OP_NOP, MIPS_EXCP_RI)
TEST_R_EXCP(invalid_2, 0x13, 20, 1920, 15, 1049, 15, ALU_OP_NOP, MIPS_EXCP_RI)

// -------- Instruction decode I type ALU tests --------------------------------
#define TEST_I_ALU_NO_EXCP(NAME, OPC, RS, DATA_RS, IMM, RT, EXP_ALU_OP)  \
	DEFINE_TEST(id_i_alu_no_excp_##NAME) {                               \
		mips_exception_t excp = {.raised = false};                       \
		id_test_i_type_alu(OPC, RS, DATA_RS, IMM, RT, EXP_ALU_OP, excp); \
	}

#define TEST_I_ALU_EXCP(NAME, OPC, RS, DATA_RS, IMM, RT, EXP_ALU_OP, EXCP_CAUSE) \
	DEFINE_TEST(id_i_alu_excp_##NAME) {                                          \
		mips_exception_t excp = {.raised = true, .cause = EXCP_CAUSE};           \
		id_test_i_type_alu(OPC, RS, DATA_RS, IMM, RT, EXP_ALU_OP, excp);         \
	}

// name, opcode, rs, *rs, rt, immediate, expected alu op
TEST_I_ALU_NO_EXCP(addiu, MIPS_OPC_ADDIU, 10, 230, (uint16_t) -12, 10, ALU_OP_ADD)
TEST_I_ALU_NO_EXCP(andi, MIPS_OPC_ANDI, 5, 230, 0xF0F0, 10, ALU_OP_AND)
TEST_I_ALU_NO_EXCP(ori, MIPS_OPC_ORI, 9, 100, 50, 29, ALU_OP_OR)

// name, opcode, rs, *rs, rt, immediate, expected alu op, expected exception cause
TEST_I_ALU_EXCP(invalid_1, 0x21, 9, 100, 50, 29, ALU_OP_NOP, MIPS_EXCP_RI)
TEST_I_ALU_EXCP(invalid_2, 0x50, 9, 100, 50, 29, ALU_OP_NOP, MIPS_EXCP_RI)
TEST_I_ALU_EXCP(invalid_3, 0x31, 9, 100, 50, 29, ALU_OP_NOP, MIPS_EXCP_RI)

// -------- Instruction decode I type branch tests -----------------------------
#define TEST_I_BRANCH(NAME, OPC, RS, DATA_RS, RT, DATA_RT, BRANCH_ADDR, SHOULD_BRANCH)    \
	DEFINE_TEST(id_i_branch_##NAME) {                                                     \
		id_test_i_type_branch(OPC, RS, DATA_RS, RT, DATA_RT, BRANCH_ADDR, SHOULD_BRANCH); \
	}

// opcode, rs, *rs, rt, *rt, branch address, should it branch
TEST_I_BRANCH(beq_taken, MIPS_OPC_BEQ, 2, 12, 5, 12, 100, true)
TEST_I_BRANCH(beq_not_taken, MIPS_OPC_BEQ, 2, 12, 5, 13, 100, false)
TEST_I_BRANCH(bne_taken, MIPS_OPC_BNE, 2, 12, 5, 20, 100, true)
TEST_I_BRANCH(bne_not_taken, MIPS_OPC_BEQ, 2, 12, 5, 13, 100, false)
TEST_I_BRANCH(neg_branch, MIPS_OPC_BNE, 1, 1, 2, 0, -24444, true)

// -------- Instruction decode I type memory tests -----------------------------
// opcode, rs, *rs, rt, *rt, immediate
DEFINE_TEST(id_i_mem_lhu) { id_test_i_type_mem(MIPS_OPC_LHU, 18, 100, 23, 1000, -10); }
DEFINE_TEST(id_i_mem_sh) { id_test_i_type_mem(MIPS_OPC_LHU, 18, 100, 23, 1000, -10); }

// -------- Instruction decode J type tests ------------------------------------
// jump address
DEFINE_TEST(id_j_1) { id_test_j_type(0x2008007); }
DEFINE_TEST(id_j_2) { id_test_j_type(0x0001025); }

// -------- Execute tests ------------------------------------------------------
// *rs, *rt, op, expected_result
DEFINE_TEST(ex_reg_nop) { ex_test_reg_no_fwd(14, 15, ALU_OP_NOP, 14); }
DEFINE_TEST(ex_reg_add) { ex_test_reg_no_fwd(14, 15, ALU_OP_ADD, 29); }
DEFINE_TEST(ex_reg_and) { ex_test_reg_no_fwd(0x0FFFFF00, 0x00FFFFF0, ALU_OP_AND, 0x00FFFF00); }
DEFINE_TEST(ex_reg_or) { ex_test_reg_no_fwd(0x0FFFFF00, 0x00FFFFF0, ALU_OP_OR, 0x0FFFFFF0); }

// *rs, *rt, op, expected_result
DEFINE_TEST(ex_imm_nop) { ex_test_imm_no_fwd(14, 15, ALU_OP_NOP, 14); }
DEFINE_TEST(ex_imm_add) { ex_test_imm_no_fwd(14, 15, ALU_OP_ADD, 29); }
DEFINE_TEST(ex_imm_and) { ex_test_imm_no_fwd(0x0FFFFF00, 0xFFF0, ALU_OP_AND, 0x0000FF00); }
DEFINE_TEST(ex_imm_or) { ex_test_imm_no_fwd(0x0FFFFF01, 0xFFF0, ALU_OP_OR, 0x0FFFFFF1); }

// -------- Memory tests -------------------------------------------------------
// mem_size, read value, read address
DEFINE_TEST(mem_read_valid) { mem_test_read(512, 62031, 20); }
DEFINE_TEST(mem_read_unaligned) { mem_test_read(512, 62031, 21); }
DEFINE_TEST(mem_read_page_fault) { mem_test_read(10, 62031, 12); }

// mem_size, write value, write address
DEFINE_TEST(mem_write_valid) { mem_test_write(512, 64201, 100); }
DEFINE_TEST(mem_write_unaligned) { mem_test_write(512, 1830, 21); }
DEFINE_TEST(mem_write_page_fault) { mem_test_write(10, 1830, 12); }

// -------- Writeback tests ----------------------------------------------------
// value, register
DEFINE_TEST(wb_1) { wb_test(0xCA7CA769, 17); }
DEFINE_TEST(wb_2) { wb_test(0x54AAAAAA, 4); }
DEFINE_TEST(wb_zero) { wb_test(0xFEEDACA7, 0); }
