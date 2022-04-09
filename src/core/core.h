#ifndef CORE_H
#define CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "arch_state.h"
#include "pipeline_regs.h"
#include "util/util.h"

typedef struct {
	// Architectural state
	mips_state_t state;

	// Config/metrics
	bool     delay_slots;
	uint32_t cycle;

	// Memory
	span_t instr_mem;
	span_t data_mem;

	// Pipeline registers
	mips_pipeline_regs_t regs;
} mips_core_t;

typedef struct {
	int _;
} mips_result_t;

void mips_core_init(mips_core_t* core, span_t instr_mem, span_t data_mem, bool delay_slots);

// Execute instructions until one generates a trap
mips_result_t mips_core_run(mips_core_t* core);

// Execute one clock cycle
mips_result_t mips_core_cycle(mips_core_t* core);

// Pipeline stages, defined in separate header files
if_id_reg_t  instruction_fetch(const mips_state_t* arch_state, span_t instr_mem);
id_ex_reg_t  instruction_decode(const mips_pipeline_regs_t* regs, const mips_state_t* arch_state);
ex_mem_reg_t execute(const mips_pipeline_regs_t* regs);
mem_wb_reg_t memory(const ex_mem_reg_t* ex_mem, span_t data_mem);
void         writeback(const mem_wb_reg_t* mem_wb, mips_state_t* arch_state);

void if_id_reg_init(if_id_reg_t* if_id);
void id_ex_reg_init(id_ex_reg_t* id_ex);
void ex_mem_reg_init(ex_mem_reg_t* ex_mem);
void mem_wb_reg_init(mem_wb_reg_t* mem_wb);

#ifdef __cplusplus
}
#endif

#endif
