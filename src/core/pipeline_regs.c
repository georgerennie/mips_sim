#include "pipeline_regs.h"
#include <stdarg.h>
#include "util/log.h"

static inline void log_reg_name(const char* name, size_t gap) {
	log_msg("%11s: ", name);
	if (gap > 0) { log_msg("%*c", gap * 10, ' '); }
}

static inline void log_reg_hex(const char* name, size_t gap, size_t n, ...) {
	log_reg_name(name, gap);

	va_list argp;
	va_start(argp, n);
	for (size_t i = 0; i < n; i++) { log_msg("%08x  ", va_arg(argp, uint32_t)); }
	va_end(argp);
	log_msg("\n");
}

static inline void log_reg_dec(const char* name, size_t gap, size_t n, ...) {
	log_reg_name(name, gap);

	va_list argp;
	va_start(argp, n);
	for (size_t i = 0; i < n; i++) { log_msg("%8d  ", va_arg(argp, uint32_t)); }
	va_end(argp);
	log_msg("\n");
}

static inline void log_reg_str(const char* name, size_t gap, size_t n, ...) {
	log_reg_name(name, gap);

	va_list argp;
	va_start(argp, n);
	for (size_t i = 0; i < n; i++) { log_msg("%08s  ", va_arg(argp, const char*)); }
	va_end(argp);
	log_msg("\n");
}

void log_pipeline_regs(const mips_pipeline_regs_t* regs) {
	log_reg_str("stage", 0, 4, "IF/ID  ", "ID/EX  ", "EX/MEM ", "MEM/WB ");
	log_reg_hex("instruction", 0, 1, regs->if_id.instruction);
	log_reg_hex("address", 0, 1, regs->if_id.address);

	log_reg_dec("rs", 1, 1, regs->id_ex.reg_rs);
	log_reg_hex("*rs", 1, 1, regs->id_ex.data_rs);
	log_reg_dec("rt", 1, 1, regs->id_ex.reg_rt);
	log_reg_hex("*rt", 1, 2, regs->id_ex.data_rt, regs->ex_mem.data_rt);
	log_reg_hex("immediate", 1, 1, regs->id_ex.immediate);

	log_reg_dec("eval branch", 1, 1, regs->id_ex.eval_branch);
	log_reg_dec("branch", 1, 1, regs->id_ex.branch);
	log_reg_hex("branch addr", 1, 1, regs->id_ex.branch_address);

	log_reg_str("alu op", 1, 1, alu_op_to_str(regs->id_ex.alu_op));
	log_reg_str("alu b src", 1, 1, alu_src_to_str(regs->id_ex.alu_b_src));

	log_reg_str(
	    "mem access", 1, 2, mem_access_to_str(regs->id_ex.ex_mem.access_type),
	    mem_access_to_str(regs->ex_mem.access_type));
	log_reg_dec("mem bytes", 1, 2, regs->id_ex.ex_mem.bytes, regs->ex_mem.bytes);

	log_reg_dec(
	    "wb reg", 1, 3, regs->id_ex.ex_mem.mem_wb.reg, regs->ex_mem.mem_wb.reg, regs->mem_wb.reg);
	log_reg_hex(
	    "wb result", 1, 3, regs->id_ex.ex_mem.mem_wb.result, regs->ex_mem.mem_wb.result,
	    regs->mem_wb.result);
}

const char* mem_access_to_str(memory_access_t access) {
	switch (access) {
		case MEM_ACCESS_NONE: return "none";
		case MEM_ACCESS_READ_UNSIGNED: return "readu";
		case MEM_ACCESS_READ_SIGNED: return "reads";
		case MEM_ACCESS_WRITE: return "write";
	}
}

const char* alu_op_to_str(alu_op_t op) {
	switch (op) {
		case ALU_OP_NOP: return "nop";
		case ALU_OP_ADD: return "add";
		case ALU_OP_AND: return "and";
		case ALU_OP_OR: return "or";
	}
}

const char* alu_src_to_str(alu_src_t src) {
	switch (src) {
		case ALU_SRC_DATA_B: return "data b";
		case ALU_SRC_IMM: return "imm";
	}
}
