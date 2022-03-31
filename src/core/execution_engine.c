#include "execution_engine.h"
#include "util/log.h"

static inline uint32_t compute_alu_op(alu_op_t op, uint32_t a, uint32_t b) {
	switch (op) {
		case ALU_OP_NOP: return a;

		case ALU_OP_ADD: return a + b;
		case ALU_OP_AND: return a & b;
		case ALU_OP_OR: return a | b;

		default: log_assert_fail("Unrecognised ALU operation %d\n", op); break;
	}
}

memory_access_bundle_t execute_instruction(const execute_bundle_t* bundle) {
	memory_access_bundle_t mem = bundle->mem;
	mem.wb.value               = compute_alu_op(bundle->op, bundle->arg_a, bundle->arg_b);
	return mem;
}

inline void execute_bundle_init(execute_bundle_t* bundle) {
	bundle->op    = ALU_OP_NOP;
	bundle->arg_a = 0;
	bundle->arg_b = 0;
	memory_access_bundle_init(&bundle->mem);
}
