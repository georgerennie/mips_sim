#ifndef _EXECUTION_ENGINE_H_
#define _EXECUTION_ENGINE_H_

#include <inttypes.h>
#include "memory_access.h"

typedef enum {
	ALU_OP_NOP,

	ALU_OP_ADD,
	ALU_OP_SUB,

	ALU_OP_AND,
	ALU_OP_OR,
	ALU_OP_XOR,
	ALU_OP_NOR,
} alu_op_t;

typedef struct {
	alu_op_t op;           // ALU Operation
	uint32_t arg_a, arg_b; // Arguments to the ALU

	memory_access_bundle_t mem;
} execute_bundle_t;

memory_access_bundle_t execute_instruction(const execute_bundle_t* bundle);
void execute_bundle_init(execute_bundle_t* bundle);

#endif
