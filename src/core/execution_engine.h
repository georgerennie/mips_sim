#ifndef EXECUTION_ENGINE_H
#define EXECUTION_ENGINE_H

#include <inttypes.h>
#include "memory_access.h"

typedef enum {
	ALU_OP_NOP = 0,

	ALU_OP_ADD,
	ALU_OP_AND,
	ALU_OP_OR,
} alu_op_t;

typedef enum {
	ALU_FWD_SRC_NONE = 0,
	ALU_FWD_SRC_EXEC,
	ALU_FWD_SRC_MEM,
} alu_fwd_src_t;

typedef struct {
	alu_op_t op;           // ALU Operation
	uint32_t arg_a, arg_b; // Arguments to the ALU

	alu_fwd_src_t fwd_a, fwd_b;

	memory_access_bundle_t mem;
} execute_bundle_t;

memory_access_bundle_t execute_instruction(
    const execute_bundle_t* bundle, uint32_t exec_res, uint32_t mem_res);
void execute_bundle_init(execute_bundle_t* bundle);

#endif
