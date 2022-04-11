#include "common/log.h"
#include "core.h"
#include "forwarding_unit.h"

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

ex_mem_reg_t execute(const mips_pipeline_regs_t* regs) {
	const id_ex_reg_t* id_ex = &regs->id_ex;
	const uint32_t     rs    = get_fwd_value(regs, id_ex->reg_rs, id_ex->data_rs);
	const uint32_t     rt    = get_fwd_value(regs, id_ex->reg_rt, id_ex->data_rt);
	const uint32_t     b     = (id_ex->alu_b_src == ALU_SRC_IMM) ? id_ex->immediate : rt;

	ex_mem_reg_t ex_mem  = id_ex->ex_mem;
	ex_mem.mem_wb.result = compute_alu_op(id_ex->alu_op, rs, b);
	log_dbgi("Execution result: %lu\n", ex_mem.mem_wb.result);

	// Propagate forwarded rt through to memory stage for store instructions
	ex_mem.data_rt = rt;

	return ex_mem;
}
