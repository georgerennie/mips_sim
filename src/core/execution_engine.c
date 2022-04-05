#include "execution_engine.h"
#include "util/log.h"

static inline void log_dbg_exec(uint32_t a, const char* op, uint32_t b) {
	log_dbgi("Executing %d %s %d\n", a, op, b);
}

static inline uint32_t compute_alu_op(alu_op_t op, uint32_t a, uint32_t b) {
	switch (op) {
		case ALU_OP_NOP: log_dbg_exec(a, "nop", 0); return a;

		case ALU_OP_ADD: log_dbg_exec(a, "+", b); return a + b;
		case ALU_OP_AND: log_dbg_exec(a, "&", b); return a & b;
		case ALU_OP_OR: log_dbg_exec(a, "|", b); return a | b;

		default: log_assert_fail("Unrecognised ALU operation %d\n", op); break;
	}
}

static inline void log_dbg_fwd(char dst, const char* src) {
	log_dbgi("Forwarding %c from %s", dst, src);
}

memory_access_bundle_t execute_instruction(
    const execute_bundle_t* bundle, uint32_t exec_res, uint32_t mem_res) {
	uint32_t a;
	switch (bundle->fwd_a) {
		case ALU_FWD_SRC_EXEC:
			log_dbg_fwd('a', "exec");
			a = exec_res;
			break;
		case ALU_FWD_SRC_MEM:
			log_dbg_fwd('a', "exec");
			a = mem_res;
			break;
		default: a = bundle->arg_a; break;
	}

	uint32_t b;
	switch (bundle->fwd_b) {
		case ALU_FWD_SRC_EXEC:
			log_dbg_fwd('b', "exec");
			b = exec_res;
			break;
		case ALU_FWD_SRC_MEM:
			log_dbg_fwd('b', "exec");
			b = mem_res;
			break;
		default: b = bundle->arg_b; break;
	}

	memory_access_bundle_t mem = bundle->mem;
	mem.wb.value               = compute_alu_op(bundle->op, a, b);
	log_dbgi("Execution result: %d\n", mem.wb.value);
	return mem;
}

inline void execute_bundle_init(execute_bundle_t* bundle) {
	bundle->op    = ALU_OP_NOP;
	bundle->arg_a = 0;
	bundle->arg_b = 0;
	bundle->fwd_a = ALU_FWD_SRC_NONE;
	bundle->fwd_b = ALU_FWD_SRC_NONE;
	memory_access_bundle_init(&bundle->mem);
}
