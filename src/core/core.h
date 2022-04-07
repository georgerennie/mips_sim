#ifndef CORE_H
#define CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "arch_state.h"
#include "execute.h"
#include "memory.h"
#include "util/util.h"
#include "writeback.h"

typedef enum {
	MIPS_STAGE_NONE = -1,

	MIPS_STAGE_IF  = 0,
	MIPS_STAGE_ID  = 1,
	MIPS_STAGE_EX  = 2,
	MIPS_STAGE_MEM = 3,
	MIPS_STAGE_WB  = 4,
} mips_core_stage_t;

typedef struct {
	mips_state_t state;

	uint32_t cycle;

	span_t instr_mem;
	span_t data_mem;

	uint32_t     if_id_instruction;
	id_ex_reg_t  id_ex;
	ex_mem_reg_t ex_mem;
	mem_wb_reg_t mem_wb;
} mips_core_t;

typedef struct {
	int _;
} mips_result_t;

void mips_core_init(mips_core_t* core, span_t instr_mem, span_t data_mem);

// Execute instructions until one generates a trap
mips_result_t mips_core_run(mips_core_t* core);

// Execute one clock cycle
mips_result_t mips_core_cycle(mips_core_t* core);

#ifdef __cplusplus
}
#endif

#endif
