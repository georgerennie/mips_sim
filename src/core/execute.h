#ifndef EXECUTE_H
#define EXECUTE_H

#include <inttypes.h>
#include "memory.h"

typedef enum {
	ALU_OP_NOP = 0,

	ALU_OP_ADD,
	ALU_OP_AND,
	ALU_OP_OR,
} alu_op_t;

typedef enum {
	ALU_SRC_DATA_B = 0, // Use value read from 2nd register in ID
	ALU_SRC_IMM,        // Use immediate generated in ID
} alu_src_t;

typedef enum {
	EX_FWD_SRC_NONE = 0,
	EX_FWD_SRC_EXEC,
	EX_FWD_SRC_MEM,
} ex_fwd_src_t;

typedef struct {
	// Data
	uint32_t data_rs, data_rt; // Arguments to the ALU
	uint8_t  reg_rs, reg_rt;   // Registers of data_rs and data_rt for forwarding unit
	uint32_t immediate;

	uint32_t ex_mem_result, mem_wb_result; // Values forwarded from EX/MEM stages

	// Control
	alu_op_t     alu_op;       // ALU Operation
	alu_src_t    alu_b_src;    // Select data_b/forwarded value or immediate as b input
	ex_fwd_src_t fwd_a, fwd_b; // Choose forwarded source

	// Pipeline passthrough
	ex_mem_reg_t ex_mem;
} id_ex_reg_t;

ex_mem_reg_t execute(const id_ex_reg_t* id_ex);
void         id_ex_reg_init(id_ex_reg_t* id_ex);

#endif
