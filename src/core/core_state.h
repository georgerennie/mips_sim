#ifndef _CORE_STATE_H_
#define _CORE_STATE_H_

#include <stdbool.h>
#include "arch_state.h"
#include "execution_engine.h"
#include "memory_access.h"
#include "util/util.h"
#include "writeback.h"

typedef enum {
	MIPS_STAGE_IF  = 0,
	MIPS_STAGE_ID  = 1,
	MIPS_STAGE_EX  = 2,
	MIPS_STAGE_MEM = 3,
	MIPS_STAGE_WB  = 4,
} mips_core_stage_t;

typedef struct {
	mips_state_t state;

	span_t instr_mem;
	span_t data_mem;

	// True for each stage if they are stalling
	bool stalls[5];

	// Current trap generated in pipeline. Traps can only be generated in the
	// fetch and decode stages. The trap is not returned from mips_core_cycle
	// until it is flushed through the pipeline
	mips_trap_t trap;
	// The current stage of the earliest trap in the pipeline. MIPS_STAGE_IF if there are
	// no traps in the pipeline
	mips_core_stage_t trap_stage;

	uint32_t               decoded_instruction;
	execute_bundle_t       exec_bundle;
	memory_access_bundle_t mem_bundle;
	writeback_bundle_t     wb_bundle;
} mips_core_t;

#endif
