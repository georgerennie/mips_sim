#include "execute.h"
#include "util/log.h"

static inline void log_dbg_exec(uint32_t a, const char* op, uint32_t b) {
	log_dbgi("Executing %lu %s %lu\n", a, op, b);
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

ex_mem_reg_t execute(const id_ex_reg_t* id_ex) {
	const uint32_t a = (id_ex->fwd_a == EX_FWD_SRC_EXEC)  ? id_ex->ex_mem_result
	                   : (id_ex->fwd_a == EX_FWD_SRC_MEM) ? id_ex->mem_wb_result
	                                                      : id_ex->data_a;
	const uint32_t b = (id_ex->alu_b_src == ALU_SRC_IMM)   ? id_ex->immediate
	                   : (id_ex->fwd_a == EX_FWD_SRC_EXEC) ? id_ex->ex_mem_result
	                   : (id_ex->fwd_a == EX_FWD_SRC_MEM)  ? id_ex->mem_wb_result
	                                                       : id_ex->data_a;

	ex_mem_reg_t ex_mem = id_ex->ex_mem;
	ex_mem.mem_wb.value = compute_alu_op(id_ex->alu_op, a, b);
	log_dbgi("Execution result: %lu\n", ex_mem.mem_wb.value);
	return ex_mem;
}

void id_ex_reg_init(id_ex_reg_t* id_ex) {
	id_ex->data_a        = 0;
	id_ex->data_b        = 0;
	id_ex->immediate     = 0;
	id_ex->ex_mem_result = 0;
	id_ex->mem_wb_result = 0;

	id_ex->alu_op    = ALU_OP_NOP;
	id_ex->alu_b_src = ALU_SRC_DATA_B;
	id_ex->fwd_a     = EX_FWD_SRC_NONE;
	id_ex->fwd_b     = EX_FWD_SRC_NONE;
	ex_mem_reg_init(&id_ex->ex_mem);
}
