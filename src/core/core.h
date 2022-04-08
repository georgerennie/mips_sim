#ifndef CORE_H
#define CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "arch_state.h"
#include "execute.h"
#include "instruction_decode.h"
#include "memory.h"
#include "util/util.h"
#include "writeback.h"

typedef enum {
	MIPS_STAGE_NONE = -1,

	MIPS_STAGE_IF = 0,
	MIPS_STAGE_ID,
	MIPS_STAGE_EX,
	MIPS_STAGE_MEM,
	MIPS_STAGE_WB,

	MIPS_STAGE_NUM,
} mips_core_stage_t;

typedef struct {
	if_id_reg_t  if_id;
	id_ex_reg_t  id_ex;
	ex_mem_reg_t ex_mem;
	mem_wb_reg_t mem_wb;

	// If a stage is stalled, it doesnt write out its value
	bool stalls[MIPS_STAGE_NUM - 1];
} mips_pipeline_regs_t;

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

#ifdef __cplusplus
}
#endif

#endif
