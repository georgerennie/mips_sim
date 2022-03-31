#ifndef _CORE_H_
#define _CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "arch_state.h"
#include "execution_engine.h"
#include "instruction_decoder.h"
#include "memory_access.h"
#include "util/util.h"
#include "writeback.h"

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
	// The current stage of the earliest trap in the pipeline. 0 if there are
	// no traps in the pipeline
	uint8_t trap_stage;

	uint32_t               decoded_instruction;
	execute_bundle_t       exec_bundle;
	memory_access_bundle_t mem_bundle;
	writeback_bundle_t     wb_bundle;
} mips_core_t;

void mips_core_init(mips_core_t* core, span_t instr_mem, span_t data_mem);

// Execute instructions until one generates a trap
mips_trap_t mips_core_run(mips_core_t* core);

// Execute one clock cycle
mips_trap_t mips_core_cycle(mips_core_t* core);

#ifdef __cplusplus
}
#endif

#endif
