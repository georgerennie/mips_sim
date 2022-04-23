#ifndef PIPELINE_TEST_H
#define PIPELINE_TEST_H

#include "common/instruction.h"
#include "common/util.h"
#include "core/core.h"

void id_test_r_type(
    mips_funct_t funct, mips_reg_idx_t rs, uint32_t data_rs, mips_reg_idx_t rt, uint32_t data_rt,
    mips_reg_idx_t rd, alu_op_t expected_alu_op, mips_exception_t expected_exception);

void id_test_i_type_alu(
    mips_opcode_t opc, mips_reg_idx_t rs, uint32_t data_rs, uint16_t immediate, mips_reg_idx_t rt,
    alu_op_t expected_alu_op, mips_exception_t expected_exception);
void id_test_i_type_branch(
    mips_opcode_t opc, mips_reg_idx_t rs, uint32_t data_rs, mips_reg_idx_t rt, uint32_t data_rt,
    int16_t branch_addr, bool should_branch);
void id_test_i_type_mem(
    mips_opcode_t opc, mips_reg_idx_t rs, uint32_t data_rs, mips_reg_idx_t rt, uint32_t data_rt,
    int16_t immediate);

void id_test_j_type(mips_j_address_t address);

void ex_test_reg_no_fwd(uint32_t data_rs, uint32_t data_rt, alu_op_t op, uint32_t expected_result);
void ex_test_imm_no_fwd(uint32_t data_rs, uint32_t imm, alu_op_t op, uint32_t expected_result);

void mem_test_read(uint32_t mem_size, uint16_t value, uint32_t address);
void mem_test_write(uint32_t mem_size, uint16_t value, uint32_t address);

void wb_test(uint32_t value, mips_reg_idx_t reg);

#endif
