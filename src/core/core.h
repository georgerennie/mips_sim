#ifndef CORE_H
#define CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "common/arch_structs.h"
#include "common/util.h"
#include "pipeline_regs.h"

typedef struct {
	// Architectural state
	mips_state_t state;

	// Config
	mips_config_t config;

	// Metrics/Metadata
	uint32_t cycle;
	uint32_t instruction_number;

	// Pipeline registers
	mips_pipeline_regs_t regs;
} mips_core_t;

typedef struct {
	int _;
} mips_result_t;

void mips_core_init(mips_core_t* core, mips_config_t config);

// Execute instructions until one generates a trap
// mips_retire_metadata_t mips_core_run(mips_core_t* core);

// Execute until one new instruction retires
mips_retire_metadata_t mips_core_run_one(mips_core_t* core);

// Execute one clock cycle
mips_retire_metadata_t mips_core_cycle(mips_core_t* core);

// Pipeline stages, defined in separate header files
if_id_reg_t  instruction_fetch(const mips_state_t* arch_state, span_t instr_mem);
id_ex_reg_t  instruction_decode(const mips_pipeline_regs_t* regs, const mips_state_t* arch_state);
ex_mem_reg_t execute(const mips_pipeline_regs_t* regs);
mem_wb_reg_t memory(const ex_mem_reg_t* ex_mem, span_t data_mem);
void         writeback(const mem_wb_reg_t* mem_wb, mips_state_t* arch_state);

#ifdef __cplusplus
}
#endif

#endif
