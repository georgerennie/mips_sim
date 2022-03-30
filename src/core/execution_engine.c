#include "execution_engine.h"
#include <assert.h>
#include <inttypes.h>

static inline uint32_t compute_alu_op(alu_op_t op, uint32_t a, uint32_t b) {
	switch (op) {
		case ALU_OP_NOP: return a;

		case ALU_OP_ADD: return a + b;
		case ALU_OP_SUB: return a - b;

		case ALU_OP_AND: return a & b;
		case ALU_OP_OR: return a | b;
		case ALU_OP_XOR: return a ^ b;
		case ALU_OP_NOR: return ~(a | b);

		default: assert(0); break;
	}
}

execution_bundle_t execute_instruction(const decode_bundle_t *decode) {
	execution_bundle_t bundle = {
	    .result          = compute_alu_op(decode->alu_op, decode->alu_arg_a, decode->alu_arg_b),
	    .mem_access_type = decode->mem_access_type,
	    .mem_bytes       = decode->mem_bytes,
	    .writeback_reg   = decode->writeback_reg,
	};

	return bundle;
}
